#pragma once

#include "Hittable.h"

class Sphere : public Hittable {
  public:
    Sphere(vec3 center, double radius) : center(center), radius(radius), Hittable() {}
    Sphere(vec3 center, double radius, const Material *material, vec3 emission = vec3())
      : center(center), radius(radius), Hittable(material, emission) {}

    bool hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const override;
    void get_tex_coords(const vec3 &pos, double &u, double &v) const override;
  private:
    vec3 center;
    double radius;
};

bool Sphere::hit(const Ray &ray, double t_min, double t_max, hit_record &rec) const {
  vec3 l = center - ray.origin;
  double b = dot(l, ray.dir);
  double det = b * b - dot(l, l) + radius * radius;
  if (det < 1e-6) {
    return false;
  }
  det = sqrt(det);
  double tmp = b - det;
  if (tmp > t_min && tmp < t_max) {
    rec.t = tmp;
    rec.pos = at(ray, tmp);
    this->get_tex_coords((rec.pos - center) / radius, rec.u, rec.v);
    rec.normal = normalize(rec.pos - center);
    rec.object = this;
    return true;
  }
  tmp = b + det;
  if (tmp > t_min && tmp < t_max) {
    rec.t = tmp;
    rec.pos = at(ray, tmp);
    this->get_tex_coords((rec.pos - center) / radius, rec.u, rec.v);
    rec.normal = normalize(rec.pos - center);
    rec.object = this;
    return true;
  }
  return false;
}

void Sphere::get_tex_coords(const vec3 &pos, double &u, double &v) const {
  double phi = atan2(pos.x, pos.y);
  double theta = asin(pos.z);
  u = 1 - (phi + M_PI) / (2 * M_PI);
  v = (theta + M_PI / 2) * M_1_PI;
}