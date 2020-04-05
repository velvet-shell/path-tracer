#pragma once

#include <vector>

#include "Hittable.h"
#include "Sphere.h"

class Envmap : public Sphere {
  public:
    Envmap(const Texture *texture) : texture(texture), Sphere(vec3(), 1e20) {}
    
    const Texture *texture = nullptr;
};

class Scene {
  public:
    Scene() {}

    void add(const Hittable *object);
    void add(const Envmap *map);
    bool hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const;
    vec3 get_background(const Ray &ray) const;
  private:
    std::vector<const Hittable *> objects;
    const Envmap *envmap = nullptr;
};

void Scene::add(const Hittable *object) {
  objects.push_back(object);
}

void Scene::add(const Envmap *map) {
  envmap = map;
}

bool Scene::hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const {
  double closest = t_max;
  hit_record temp_rec;
  for (const auto& object : objects) {
    if (object->hit(ray, t_min, closest, temp_rec)) {
      closest = temp_rec.t;
      rec = temp_rec;
    }
  }
  return closest < t_max;
}

vec3 Scene::get_background(const Ray &ray) const {
  hit_record rec;
  envmap->hit(ray, 0, 1e21, rec);
  return envmap->texture->get_texture(rec.u, rec.v);
}