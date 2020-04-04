#include <iostream>

#include "utility.h"
#include <stb_image.h>

#include "Lambert.h"
#include "IdealSpecular.h"
#include "Phong.h"
#include "CookTorrance.h"

#include "Sphere.h"
#include "Triangle.h"

#include "Scene.h"
#include "Camera.h"


int map_width, map_height, nrChannels;
unsigned char *map = stbi_load("textures/pier.jpg", &map_width, &map_height, &nrChannels, 0);                   

vec3 radiance(const Scene *scene, const Ray &ray, int depth, unsigned short* seed) {
  hit_record hit;
  if (!scene->hit(ray, 1e-4, 1e20, hit)) {
    vec3 l = -ray.origin;
    double coef = dot(l, ray.dir);
    double det = coef * coef - dot(l, l) + 1e20 * 1e20;
    det = sqrt(det);
    double tmp = coef + det;
    vec3 p = at(ray, tmp) / 1e20;

    double phi = atan2(p.x, p.y);
    double theta = asin(p.z);
    double u = 1 - (phi + M_PI) / (2 * M_PI);
    double v = (theta + M_PI / 2) * M_1_PI;
    if (map == nullptr)
      return vec3(0,0,1);

    auto i = static_cast<int>(u * map_width);
    auto j = static_cast<int>((1 - v) * map_height - 0.001);

    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > map_width - 1)  i = map_width - 1;
    if (j > map_height - 1) j = map_height - 1;

    auto r = map[3 * i + 3 * map_width * j + 0] / 255.0;
    auto g = map[3 * i + 3 * map_width * j + 1] / 255.0;
    auto b = map[3 * i + 3 * map_width * j + 2] / 255.0;

    return vec3(r, g, b);
    // return vec3(0.5, 0.7, 1.0);
  }
  vec3 wo = normalize(-ray.dir);
  hit.normal = dot(hit.normal, ray.dir) < 0 ? hit.normal : -hit.normal;

  vec3 wi, attenuation;
  wi = hit.object->material->sample(wo, &hit, seed, attenuation);

  double p = max(attenuation);
  if (depth > 5 || !p) {
    if (erand48(seed) < p) {
      attenuation /= p;
    } else {
      return hit.object->emission;
    }
  }
  return hit.object->emission + attenuation * radiance(scene, Ray(hit.pos, wi), depth + 1, seed);
}

int main(int argc, char* argv[]) {
  int w = 1920, h = 1080;
  int samples = 1;
  if (argc == 2) samples = atoi(argv[1]) / 4;

  Camera cam(vec3(0, -1, 0), vec3(0, 0, 0), vec3(0, 0, 1), M_PI / 2, (double) w / h);

  Scene scene;

  // scene.add(new Triangle(vec3(-1, 0, 1), vec3(1, 0, 1), vec3(0, 0, 2.4),
  //                        new Lambert(vec3(0.0, 0.0, 0.5))));                   
  // scene.add(new Sphere(vec3(0, 0, -1e5), 1e5,
  //                      new Lambert(vec3(0.0, 0.5, 0.0))));

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
            Ray ray = cam.get_ray(((sx + 0.5 + dx) / 2 + col) / w, ((sy + 0.5 + dy) / 2 + row) / h);
            result = result + radiance(&scene, ray, 0, seed);
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
		fprintf(f, "%d %d %d ", gamma_cor(pixels[i].x), gamma_cor(pixels[i].y), gamma_cor(pixels[i].z));
  }
}