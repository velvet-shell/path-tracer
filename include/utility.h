#pragma once

#include "vec3.h"

inline double clamp(double x) {
  return x < 0 ? 0 : x > 1 ? 1 : x;
}
vec3 schlick_approximation(vec3 r, double radians) {
  return r + (1.0 - r) * pow(1.0 - radians, 5.0);
}
inline vec3 reflect(const vec3 &d, const vec3 &n) {
  return 2.f * n * dot(n, d) - d;
}
vec3 map_coords(vec3 axis, vec3 sample) {
  vec3 w = fabs(axis.x) > 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);
  vec3 u = normalize(cross(w, axis));
  vec3 v = cross(axis, u);
  return u * sample.x + v * sample.y + axis * sample.z;
}
vec3 sample_cosine_powered(vec3 axis, double power, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = pow(r2, 1.0 / (power + 1.0));
  double sin_phi = sqrt(1 - pow(r2, 2.0 / (power + 1.0)));
  vec3 input(sin_phi * cos(theta), sin_phi * sin(theta), h);
  return map_coords(axis, input);
}
vec3 sample_cosine(vec3 axis, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = sqrt(1 - r2);
  vec3 input(h * cos(theta), h * sin(theta), sqrt(r2));
  return map_coords(axis, input);
}
