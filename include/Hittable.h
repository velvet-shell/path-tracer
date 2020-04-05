#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Material.h"

class Hittable;
class Material;

struct hit_record {
  vec3 pos;
  double t;
  double u;
  double v;
  vec3 normal;
  const Hittable *object;
};

class Hittable {
  public:
    Hittable() {}
    Hittable(const Material *material, vec3 emission) : material(material), emission(emission) {}
    virtual bool hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const = 0;
    virtual void get_tex_coords(const vec3 &pos, double &u, double &v) const = 0;
  public:
    const Material *material;
    vec3 emission;
};