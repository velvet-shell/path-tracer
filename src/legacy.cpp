#include <cmath>
#include <iostream>
#include <fstream>
#include <random>

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0, 1);

enum ReflectionType { DIFF, SPEC, REFR, PHONG };

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
    double eps = 1e-4;
    if (b - det > eps) {
      return b - det;
    }
    if (b + det > eps) {
      return b + det;
    }
    return 0;
  }
  double radius;
  Vec center;
  Vec emission;
  Vec colour;
  ReflectionType type;
};

// For testing purposes
Sphere spheres[] = {//Scene: radius, position, emission, color, material 
   Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left 
   Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght 
   Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back 
   Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt 
   Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm 
   Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top 
   Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(0.8,0.6,0.2),PHONG),//Mirr 
  //  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas 
   Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite 
 }; 
int num_spheres = sizeof(spheres) / sizeof(Sphere);

inline bool scene_intersect(const Ray &ray, double &t, int &id) {
  double inf = t = 1e20;
  for (int i = 0; i < num_spheres; i++) {
    double d;
    if ((d = spheres[i].intersect(ray)) && d < t) {
      t = d;
      id = i;
    }
  }
  return t < inf;
}

Vec reflect(const Vec &d, const Vec &n) {
  return d - n * n.dot(d) * 2.f;
}

Vec uniform_sample_hemisphere() {
  double r1 = distribution(generator);
  double r2 = distribution(generator);
  double r = sqrtf(1 - r1 * r1);
  double phi = 2 * M_PI * r2; 
  return Vec(cos(phi) * r, sin(phi) * r, r1);
}

Vec cosine_sample_hemisphere() {
  double r1 = distribution(generator);
  double r2 = distribution(generator);
  double r = sqrt(1 - r1);
  double theta = 2 * M_PI * r2;
  return Vec(cos(theta) * r, sin(theta) * r, sqrt(r1));
}

Vec sample_modified_phong(double alpha) {
  double r1 = distribution(generator);
  double r2 = distribution(generator);
  double t = pow(r1, 2 / (alpha + 1));
  double theta = 2 * M_PI * r2;
  return Vec(cos(theta) * sqrt(1 - t), sin(theta) * sqrt(1 - t), sqrt(t));
}

Vec random_in_unit_sphere() {
    while (true) {
        double r1 = 2 * distribution(generator) - 1;
        double r2 = 2 * distribution(generator) - 1;
        double r3 = 2 * distribution(generator) - 1;
        Vec p(r1, r2, r3);
        if (p.dot(p) >= 1) continue;
        return p;
    }
}

Vec map_coords(Vec norm, Vec sample) {
  Vec u = (fabs(norm.x) > 0.99 ? Vec(0, 1, 0) : Vec(1, 0, 0)).cross(norm).normalize();
  Vec v = norm.cross(u);
  return u * sample.x + v * sample.y + norm * sample.z;
}

double refractive_index = 1.5;

Vec radiance(const Ray &ray, int depth) {
  double t;
  int id = 0;
  if (!scene_intersect(ray, t, id)) {
    return Vec();
  }
  const Sphere &obj = spheres[id];
  Vec hit = ray.origin + ray.dir * t;
  Vec norm = (hit - obj.center).normalize();
  Vec norm_dir = norm.dot(ray.dir) < 0 ? norm : norm * -1;
  Vec colour = obj.colour;

  double max_reflectivity = std::max(colour.x, std::max(colour.y, colour.z));
	if (++depth > 5 || !max_reflectivity) {
		if (distribution(generator) < max_reflectivity) {
			colour = colour * (1 / max_reflectivity);
    } else {
			return obj.emission;
    }
  }
  if (obj.type == DIFF) {
    Vec sample = map_coords(norm_dir, cosine_sample_hemisphere());
		return obj.emission + colour * radiance(Ray(hit, sample), depth);
	} else if (obj.type == SPEC) {
    return obj.emission + colour * radiance(Ray(hit, reflect(ray.dir, norm) + random_in_unit_sphere()), depth);
  } else if (obj.type == PHONG) {
    double alpha = 32;
    double p;
    if (distribution(generator) < p) {
      // sample Phong
      Vec sample = map_coords(norm_dir, sample_modified_phong(alpha));
      return obj.emission + colour * radiance(Ray(hit, sample), depth);
    } else {
      // sample Lambert
      Vec sample = map_coords(norm_dir, cosine_sample_hemisphere());
      return obj.emission + colour * radiance(Ray(hit, sample), depth);
    }
  } else if (obj.type == REFR) {
    Ray reflected(hit, reflect(ray.dir, norm));
    float cosi = -ray.dir.dot(norm);
    float eta = 1 / refractive_index;
    if (cosi < 0) {
      cosi = -cosi;
      eta = refractive_index;
    }
    float cost2 = 1 - eta * eta * (1 - cosi * cosi);
    if (cost2 < 0) {
      return obj.emission + colour * radiance(Ray(hit, reflect(ray.dir, norm)), depth);
    }
    Ray refracted(hit, ray.dir * eta + norm_dir * (eta * cosi - sqrt(cost2)));
    double F0 = pow(eta - 1, 2) / pow(eta + 1, 2);
    double reflected_term = F0 + (1 - F0) * pow(1 - cosi, 5);
    if (depth <= 2) {
      return obj.emission + radiance(reflected, depth) * reflected_term +
                            radiance(refracted, depth) * (1 - reflected_term);
    } else {
      double P = 0.25 + 0.5 * reflected_term;
      if (distribution(generator) < P) {
        return obj.emission + radiance(reflected, depth) * (reflected_term / P);
      } else {
        return obj.emission + radiance(refracted, depth) * ((1 - reflected_term) / (1 - P));
      }
    }
  }
  return 0;
}

double triangle_distribution() {
  double r = 2 * distribution(generator);
  if (r < 1) {
    return sqrt(r) - 1;
  } else {
    return 1 - sqrt(2 - r);
  }
}

inline double clamp(double x) {
  return x < 0 ? 0 : x > 1 ? 1 : x;
}
inline int toInt(double x) {
  return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
}

int main(int argc, char* argv[]) {
  int w = 512, h = 512;
  int samples = 1;
  if (argc == 2) samples = atoi(argv[1]) / 4;

  Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).normalize());
  Vec cx = Vec(w * 0.5135 / h, 0, 0);
  Vec cy = (cx.cross(cam.dir)).normalize() * 0.5135;

  Vec* pixels = new Vec[w * h];
  #pragma omp parallel for schedule(dynamic, 1)
  for (int row = 0; row < h; row++) {
    fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samples * 4, 100.0 * row / (h - 1));
    for (int col = 0; col < w; col++) {
      int i = (h - row - 1) * w + col;
      for (int sy = 0; sy < 2; sy++) {
        for (int sx = 0; sx < 2; sx++) {
          Vec result;
          for (int s = 0; s < samples; s++) {
            double dx = triangle_distribution();
            double dy = triangle_distribution();
            Vec d = cx * (((sx + 0.5 + dx) / 2 + col) / w - 0.5) +
                    cy * (((sy + 0.5 + dy) / 2 + row) / h - 0.5) + cam.dir;
            Ray ray(cam.origin + (d * 140), d.normalize());
            result = result + radiance(ray, 0);
          }
          result = result * (1.0 / samples);
          pixels[i] = pixels[i] + (Vec(clamp(result.x), clamp(result.y), clamp(result.z)) * (1.0 / 4));
        }
      }
    }
  }
  fprintf(stderr, "\nDone!\n");
  FILE *f = fopen("image.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i = 0; i < w*h; i++) {
		fprintf(f, "%d %d %d ", toInt(pixels[i].x), toInt(pixels[i].y), toInt(pixels[i].z));
  }
}