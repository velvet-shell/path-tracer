#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Material.h"

#include <memory>

struct hit_record {
  vec3 pos;
  double t;
  vec3 normal;
  vec3 emission;
  std::shared_ptr<Material> material;
};

class Hittable {
  public:
    Hittable(std::shared_ptr<Material> material, vec3 emission) : material(material), emission(emission) {}
    virtual bool hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const = 0;

  public:
    std::shared_ptr<Material> material;
    vec3 emission;
};