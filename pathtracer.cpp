#include <cmath>
#include <iostream>
#include <fstream>

enum ReflectionType { DIFF, SPEC, REFR };

// double erand48(unsigned short xsubi[3]) {
// 	return (double)rand() / (double)RAND_MAX;
// }

struct Vec {
  Vec(double X = 0, double Y = 0, double Z = 0) : x(X), y(Y), z(Z) {}
  Vec operator+(const Vec &b) const {
    return Vec(x + b.x, y + b.y, z + b.z);
  }
  Vec operator-(const Vec &b) const {
    return Vec(x - b.x, y - b.y, z - b.z);
  }
  Vec operator*(const Vec &b) const {
    return Vec(x * b.x, y * b.y, z * b.z);
  }
  Vec operator*(double b) const {
    return Vec(x * b, y * b, z * b);
  }
  double dot(const Vec &b) const {
    return x * b.x + y * b.y + z * b.z;
  }
  Vec cross(const Vec &b) const {
    return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
  }
  double norm() const {
    return std::sqrt(y * y + x * x + z * z);
  }
  Vec& normalize() {
    return *this = (*this) * (1 / norm());
  }
  double x, y, z; 
};

struct Ray {
  Ray(Vec Origin, Vec Dir) : origin(Origin), dir(Dir) {}
  Vec origin, dir;
};

struct Sphere {
  Sphere(double Radius, Vec Center, Vec Emission, Vec Colour, ReflectionType Type)
    : radius(Radius), center(Center), emission(Emission), colour(Colour), type(Type) {}
  double intersect(const Ray &ray) const {
    Vec l = center - ray.origin;
    double b = l.dot(ray.dir);
    double det = b * b - l.dot(l) + radius * radius;
    if (det < 0) {
      return 0;
    }
    det = sqrt(det);
    double t, eps = 1e-4;
    return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
  }
  double radius;
  Vec center;
  Vec emission;
  Vec colour;
  ReflectionType type;
};

// For testing purposes
Sphere spheres[] = {
	Sphere(1e5, Vec(1e5 + 1,40.8,81.6),  Vec(),            Vec(.75,.25,.25),DIFF), //Left
	Sphere(1e5, Vec(-1e5 + 99,40.8,81.6),Vec(),            Vec(.25,.25,.75),DIFF), //Right
	Sphere(1e5, Vec(50,40.8, 1e5),       Vec(),            Vec(.75,.75,.75),DIFF), //Back
	Sphere(1e5, Vec(50,40.8,-1e5 + 170), Vec(),            Vec(),           DIFF), //Front
	Sphere(1e5, Vec(50, 1e5, 81.6),      Vec(),            Vec(.75,.75,.75),DIFF), //Bottom
	Sphere(1e5, Vec(50,-1e5 + 81.6,81.6),Vec(),            Vec(.75,.75,.75),DIFF), //Top
	Sphere(16.5,Vec(27,16.5,47),         Vec(),            Vec(.75,.25,.25),DIFF),//Mirror
	Sphere(16.5,Vec(73,16.5,78),         Vec(),            Vec(.25,.25,.75),DIFF),//Glass
	Sphere(600, Vec(50,681.6-.27,81.6),  Vec(12,12,12),    Vec(),           DIFF)  //Light
};
int num_spheres = sizeof(spheres) / sizeof(Sphere);

inline bool scene_intersect(const Ray &ray, double &t, int &id) {
  double inf = t = 1e20;
  for (size_t i = 0; i < num_spheres; i++) {
    double d;
    if ((d = spheres[i].intersect(ray)) && d < t) {
      t = d;
      id = i;
    }
  }
  return t < inf;
}

Vec radiance(const Ray &ray, int depth, unsigned short *Xi) {
  double t;
  int id = 0;
  if (!scene_intersect(ray, t, id)) {
    return Vec();
  }
  const Sphere &obj = spheres[id];
  Vec hit = ray.origin + ray.dir * t;
  Vec norm = (hit - obj.center).normalize();
  norm = norm.dot(ray.dir) < 0 ? norm : norm * -1;
  Vec colour = obj.colour;

  double max_reflectivity = std::max(colour.x, std::max(colour.y, colour.z));
	if (++depth > 5 || !max_reflectivity) {
		if (erand48(Xi) < max_reflectivity) {
			colour = colour * (1 / max_reflectivity);
    } else {
			return obj.emission;
    }
  }
  if (obj.type == DIFF) {
		double r1 = 2 * M_PI * erand48(Xi);
		double r2 = erand48(Xi);
    double r2s = sqrt(r2);
		Vec w = norm;
		Vec u = (fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)).cross(w).normalize();
		Vec v = w.cross(u);
		Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();

		return obj.emission + colour * radiance(Ray(hit, d), depth, Xi);
	}
}

inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1 : x; }
inline int toInt(double x) { return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5); }

int main(int argc, char *argv[])
{
	int w = 512, h = 512; //image size
	int samps = argc == 2 ? atoi(argv[1]) / 4 : 1;				// # of samples(default=1)
	Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).normalize());	// camera position and direction
	Vec cx = Vec(w*.5135 / h);									// x-direction increment (uses implicit 0 for y&z)
	Vec cy = cx.cross(cam.dir).normalize()*.5135;							// y-direction increment (note cross pdt - '%')
	Vec r;														// used for colors of samples
	Vec *c = new Vec[w*h];										// The image matrix

#pragma omp parallel for schedule(dynamic, 1) private(r)		// OpenMP
	//Loop over all image pixels
	for (int y = 0; y < h; y++)	//loop over image rows
	{
		fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100.*y / (h - 1));	// print progress
		unsigned short Xi[3] = { 0,0,y*y*y };

		for (unsigned short x = 0; x < w; x++)	// Loop Columns
		{
			// For each pixel do 2x2 subsamples, and avg. samples per subpixel
			for (int sy = 0, i = (h - y - 1)*w + x; sy < 2; sy++)     // 2x2 subpixel rows
				for (int sx = 0; sx < 2; sx++, r = Vec())			  // 2x2 subpixel cols
				{
					for (int s = 0; s < samps; s++)	// Camera rays are pushed ^^^^^ forward to start in interior
					{
						//tent filter
						double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
						double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
						Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
							cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.dir;
						r = r + radiance(Ray(cam.origin + d * 140, d.normalize()), 0, Xi)*(1. / samps);
					}
					c[i] = c[i] + Vec(clamp(r.x), clamp(r.y), clamp(r.z))*.25;
				}
		}
	}
	// Write out the file to a PPM
	FILE *f = fopen("image.ppm", "w");	//write image to PPM File
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i = 0; i < w*h; i++)
		fprintf(f, "%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
}
