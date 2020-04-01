#pragma once

#include "vec3.h"

class Ray {
  public:
    Ray(vec3 origin, vec3 dir) : origin(origin), dir(dir) {}

    vec3 origin, dir;
};

inline vec3 at(const Ray &ray, double t) {
  return ray.origin + t * ray.dir;
}