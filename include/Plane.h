#pragma once

#include "Hittable.h"

class Plane : public Hittable {
  public:
    Plane(vec3 pos, vec3 norm, const Material *material, vec3 emission = vec3())
      : pos(pos), norm(norm), Hittable(material, emission) {}
    
    bool hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const override;
    void get_tex_coords(const vec3 &pos, double &u, double &v) const override;
  private:
    vec3 pos, norm;
};

bool Plane::hit(const Ray &ray, double t_min, double t_max, hit_record& rec) const {
  vec3 face_norm = dot(norm, ray.dir) < 0 ? -norm : norm;
  double denom = dot(face_norm, ray.dir);
  if (denom > 1e-6) {
    double t = dot(pos - ray.origin, face_norm) / denom;
    if (t > t_min && t < t_max) {
      rec.t = t;
      rec.pos = at(ray, t);
      rec.normal = face_norm;
      rec.object = this;
      this->get_tex_coords(rec.pos - pos, rec.u, rec.v);
      return true;
    }
  }
  return false;
}

void Plane::get_tex_coords(const vec3 &pos, double &u, double &v) const {
  vec3 mapped = map_coords(norm, pos);
  u = fmod(mapped.x, 100.0) / 100.0;
  v = fmod(mapped.y, 100.0) / 100.0;
  u = u < 0 ? 1 + u : u;
  v = v < 0 ? 1 + v : v;
}