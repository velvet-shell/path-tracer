#pragma once

#include "Hittable.h"

class Triangle: public Hittable {
  public:
    Triangle(vec3 v0, vec3 v1, vec3 v2, const Material *material, vec3 emission = vec3())
      : v0(v0), v1(v1), v2(v2), Hittable(material, emission) {}
    
    bool hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const override;
  private:
    vec3 v0, v1, v2;
};

bool Triangle::hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const {
  vec3 e1 = v1 - v0;
  vec3 e2 = v2 - v0;
  vec3 pvec = cross(ray.dir, e2);
  double det = dot(e1, pvec);

  if (det < t_min && det > -t_min) {
      return false;
  }
  vec3 tvec = ray.origin - v0;
  double u = dot(tvec, pvec) / det;
  if (u < t_min || u - 1 > t_min) {
      return false;
  }
  vec3 qvec = cross(tvec, e1);
  double v = dot(ray.dir, qvec) / det;
  if (v < t_min || u + v - 1 > t_min) {
      return false;
  }
  double tmp = dot(e2, qvec) / det;
  if (tmp > t_min && tmp < t_max) {
    rec.t = tmp;
    rec.pos = at(ray, rec.t);
    rec.normal = normalize(cross(e1, e2));
    rec.object = this;
    return true;
  }
  return false;
}