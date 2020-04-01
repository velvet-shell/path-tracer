#pragma once

#include <vector>

#include "Hittable.h"

class Scene {
  public:
    Scene() {}

    void add(const Hittable *object) { objects.push_back(object); }
    bool hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const;
  private:
    std::vector<const Hittable *> objects;
};

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