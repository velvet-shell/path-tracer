#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Material.h"

class Hittable;

struct hit_record {
  vec3 pos;
  double t;
  vec3 normal;
  const Hittable *object;
};

class Hittable {
  public:
    Hittable(const Material *material, vec3 emission) : material(material), emission(emission) {}
    virtual bool hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const = 0;

  public:
    const Material *material;
    vec3 emission;
};