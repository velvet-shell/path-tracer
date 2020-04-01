#include <iostream>

#include "vec3.h"
#include "Ray.h"
#include "Material.h"
#include "Lambert.h"
#include "IdealSpecular.h"
#include "ModifiedPhong.h"
#include "Scene.h"
#include "Sphere.h"

vec3 radiance(const Scene &scene, const Ray &ray, int depth, unsigned short* seed) {
  hit_record hit;
  if (!scene.hit(ray, 1e-4, 1e20, hit)) {
    return vec3();
  }

  vec3 wo = normalize(-ray.dir);
  hit.normal = dot(hit.normal, ray.dir) < 0 ? hit.normal : -hit.normal;

  vec3 wi = hit.material->sample(wo, hit.normal, seed);
  vec3 attenuation = hit.material->eval(wi, wo, hit.normal);

  double p = max(attenuation);
  if (depth > 5 || !p) {
    if (erand48(seed) < p) {
      attenuation /= p;
    } else {
      return hit.emission;
    }
  }
  return hit.emission + attenuation * radiance(scene, Ray(hit.pos, wi), depth + 1, seed);
}

double triangle_distribution(unsigned short *seed) {
  double r = 2 * erand48(seed);
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

  Ray cam(vec3(50, 48, 295.6), normalize(vec3(0, -0.042612, -1)));
  vec3 cx = vec3(w * 0.5135 / h, 0, 0);
  vec3 cy = normalize(cross(cx, cam.dir)) * 0.5135;

  Scene scene;
  scene.add(std::make_shared<Sphere>(vec3(50, 1e5, 81.6), 1e5, std::make_shared<Lambert>(vec3(0.75, 0.75, 0.75)), vec3()));
  scene.add(std::make_shared<Sphere>(vec3(50,-1e5+81.6,81.6), 1e5, std::make_shared<Lambert>(vec3(0.75, 0.75, 0.75)), vec3()));

  scene.add(std::make_shared<Sphere>(vec3(50,40.8, 1e5), 1e5, std::make_shared<Lambert>(vec3(0.75, 0.75, 0.75)), vec3()));
  scene.add(std::make_shared<Sphere>(vec3(50,40.8,-1e5+170), 1e5, std::make_shared<Lambert>(vec3()), vec3()));

  scene.add(std::make_shared<Sphere>(vec3(1e5+1,40.8,81.6), 1e5, std::make_shared<Lambert>(vec3(.75,.25,.25)), vec3()));
  scene.add(std::make_shared<Sphere>(vec3(-1e5+99,40.8,81.6), 1e5, std::make_shared<Lambert>(vec3(.25,.25,.75)), vec3()));

  scene.add(std::make_shared<Sphere>(vec3(50,681.6-.27,81.6), 600, std::make_shared<Lambert>(vec3()), vec3(12,12,12)));

  // scene.add(std::make_shared<Sphere>(vec3(27,16.5,47), 16.5, std::make_shared<ModifiedPhong>(vec3(0.5,0.0,0.0), vec3(0.4,0.4,0.4), 32), vec3()));
  scene.add(std::make_shared<Sphere>(vec3(27,16.5,47), 16.5, std::make_shared<Lambert>(vec3(0.8,0.6,0.2)), vec3()));
  
  vec3* pixels = new vec3[w * h];
  vec3 result;
  #pragma omp parallel for schedule(dynamic, 1) private(result)   
  for (int row = 0; row < h; row++) {
    unsigned short seed[3] = {0, 0, row * row * row};
    fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samples * 4, 100.0 * row / (h - 1));
    for (int col = 0; col < w; col++) {
      int i = (h - row - 1) * w + col;
      for (int sy = 0; sy < 2; sy++) {
        for (int sx = 0; sx < 2; sx++) {
          for (int s = 0; s < samples; s++) {
            double dx = triangle_distribution(seed);
            double dy = triangle_distribution(seed);
            vec3 d = cx * (((sx + 0.5 + dx) / 2 + col) / w - 0.5) +
                    cy * (((sy + 0.5 + dy) / 2 + row) / h - 0.5) + cam.dir;
            Ray ray(cam.origin + (d * 140), normalize(d));
            result = result + radiance(scene, ray, 0, seed);
          }
          result /= samples;
          pixels[i] = pixels[i] + vec3(clamp(result.x), clamp(result.y), clamp(result.z)) / 4;
        }
      }
    }
  }
  fprintf(stderr, "\nDone!\n");
  FILE *f = fopen("image.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i = 0; i < w * h; i++) {
		fprintf(f, "%d %d %d ", toInt(pixels[i].x), toInt(pixels[i].y), toInt(pixels[i].z));
  }
}