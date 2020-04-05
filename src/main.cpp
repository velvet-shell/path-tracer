#include <iostream>
#include <map>

#include <stb_image.h>
#include "utility.h"

#include "Lambert.h"
#include "IdealSpecular.h"
#include "Dielectric.h"
#include "Phong.h"
#include "CookTorrance.h"

#include "Sphere.h"
#include "Triangle.h"
#include "Plane.h"

#include "Scene.h"
#include "Camera.h"

int scene = 1;
int threads = 10;
std::string out_file = "./out.ppm";

vec3 radiance(const Scene *scene, const Ray &ray, int depth, unsigned short* seed) {
  hit_record hit;
  if (!scene->hit(ray, 1e-4, 1e10, hit)) {
    return scene->get_background(ray);
  }
  if (max(hit.object->emission) > 1e-4) {
    return hit.object->emission;
  }
  vec3 wo = normalize(-ray.dir);
  vec3 norm_dir = dot(hit.normal, ray.dir) < 0 ? hit.normal : -hit.normal;

  vec3 wi, attenuation;
  wi = hit.object->material->sample(wo, norm_dir, &hit, seed, attenuation);

  double p = max(attenuation);
  if (depth > 3 || !p) {
    if (erand48(seed) < p) {
      attenuation /= p;
    } else {
      return hit.object->emission;
    }
  }
  return hit.object->emission + attenuation * radiance(scene, Ray(hit.pos, wi), depth + 1, seed);
}

void render(Camera cam, Scene scene, int samples, int w, int h) {
  vec3* pixels = new vec3[w * h];
  vec3 result;
  #pragma omp parallel for num_threads(threads) private(result)  
  for (int row = 0; row < h; row++) {
    unsigned short seed[3] = {0, 0, row * row * row};
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
  FILE *f = fopen(out_file.c_str(), "w");
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int i = 0; i < w * h; i++) {
		fprintf(f, "%d %d %d ", gamma_cor(pixels[i].x), gamma_cor(pixels[i].y), gamma_cor(pixels[i].z));
  }  
}

void open_scene() {
  int w = 512, h = 512;
  Camera cam(vec3(-67.5, -67.5, 30), vec3(0, 0, 0), vec3(0, 0, 1), M_PI / 2, (double) w / h);

  Scene scene;

  int map_width, map_height, nrChannels;
  float *map = stbi_loadf("textures/pier.jpg", &map_width, &map_height, &nrChannels, 0);

  int floor_width, floor_height;
  float *floor = stbi_loadf("textures/wood.png", &floor_width, &floor_height, &nrChannels, 0);
  
  scene.add(new Envmap(new Texture(map, map_width, map_height)));
  scene.add(new Sphere(vec3(0, 500, 500), 100, new Lambert(vec3()), vec3(30, 30, 30)));
  scene.add(new Sphere(vec3(-22.5, -52.5, 15), 15, new CookTorrance(vec3(1.00, 0.86, 0.57), 0.1)));
  scene.add(new Sphere(vec3(-45, 0, 15), 15, new Dielectric()));
  scene.add(new Sphere(vec3(0, 0, 30), 15, new IdealSpecular()));
  scene.add(new Plane(vec3(0, 0, 0), vec3(0, 0, 1), new Lambert(new Texture(floor, floor_width, floor_height))));

  render(cam, scene, 1000, w, h);
}

void box_scene() {
  int w = 512, h = 512;
  Camera cam(vec3(0, -19, 0), vec3(0, 0, 0), vec3(0, 0, 1), M_PI / 2, (double) w / h);
  Scene scene;
  scene.add(new Plane(vec3(0, 20, 0),  vec3(0, -1, 0), new Lambert(vec3(0.75, 0.75, 0.75))));
  scene.add(new Plane(vec3(0, -20, 0), vec3(0, 1, 0),  new Lambert(vec3())));
  scene.add(new Plane(vec3(20, 0, 0),  vec3(-1, 0, 0), new Lambert(vec3(0.25, 0.25, 0.75))));
  scene.add(new Plane(vec3(-20, 0, 0), vec3(1, 0, 0),  new Lambert(vec3(0.75, 0.25, 0.25))));
  scene.add(new Plane(vec3(0, 0, -20), vec3(0, 0, 1),  new Lambert(vec3(0.75, 0.75, 0.75))));
  scene.add(new Plane(vec3(0, 0, 20),  vec3(0, 0, -1), new Lambert(vec3(0.75, 0.75, 0.75))));
  scene.add(new Triangle(vec3(4, -5, 18),  vec3(4, 5, 18),  vec3(6, 5, 20),  new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Triangle(vec3(2, -5, 20),  vec3(2, 5, 20),  vec3(4, 5, 18),  new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Triangle(vec3(2, -5, 20),  vec3(2, 5, 20),  vec3(0, 5, 18),  new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Triangle(vec3(-2, -5, 20), vec3(-2, 5, 20), vec3(-4, 5, 18), new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Triangle(vec3(-2, -5, 20), vec3(-2, 5, 20), vec3(0, 5, 18),  new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Triangle(vec3(-4, -5, 18), vec3(-4, 5, 18), vec3(-6, 5, 20), new Lambert(vec3()), vec3(12, 12, 12)));
  scene.add(new Sphere(vec3(15, 15, -15), 5, new Phong(vec3(0.7, 0.2, 0.2), vec3(0.7, 0.2, 0.2), 32)));
  scene.add(new Sphere(vec3(-13, 13, -13), 7, new Lambert(vec3(0.25, 0.25, 0.75))));
  render(cam, scene, 250, w, h);
}

int main(int argc, char* argv[]) {
  std::map<std::string, std::string> cmd;
    
  for (int i = 0; i < argc; i++) {
      std::string now(argv[i]);
      if (now.size() and now[0] == '-') {
          cmd[now] = i == argc - 1 ? "" : argv[i + 1];
      }
  }

  if (cmd.find("-out") != cmd.end()) {
      out_file = cmd["-out"];
  }
  if (cmd.find("-threads") != cmd.end()) {
      threads = std::atoi(cmd["-threads"].c_str());
  }
  if (cmd.find("-scene") != cmd.end()) {
      scene = std::atoi(cmd["-scene"].c_str());
  }

  if (scene == 1) {
    open_scene();
  } else if (scene == 2) {
    box_scene();
  } else {
    std::cout << "There are only 2 scenes" << std::endl;
  }
}