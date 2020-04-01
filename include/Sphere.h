#pragma once

#include "Hittable.h"

class Sphere : public Hittable {
  public:
    Sphere(vec3 center, double radius, const Material *material, vec3 emission = vec3())
      : center(center), radius(radius), Hittable(material, emission) {}

    bool hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const override;
  private:
    vec3 center;
    double radius;
};

bool Sphere::hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const {
  vec3 l = center - ray.origin;
  double b = dot(l, ray.dir);
  double det = b * b - dot(l, l) + radius * radius;
  if (det < 0) {
    return false;
  }
  det = sqrt(det);
  double tmp = b - det;
  if (tmp > t_min && tmp < t_max) {
    rec.t = tmp;
    rec.pos = at(ray, tmp);
    rec.normal = normalize(rec.pos - center);
    rec.object = this;
    return true;
  }
  tmp = b + det;
  if (tmp > t_min && tmp < t_max) {
    rec.t = tmp;
    rec.pos = at(ray, tmp);
    rec.normal = normalize(rec.pos - center);
    rec.object = this;
    return true;
  }
  return false;
}