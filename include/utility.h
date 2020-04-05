#pragma once

#include "vec3.h"

inline double clamp(double x) {
  return x < 0 ? 0 : x > 1 ? 1 : x;
}

inline int gamma_cor(double x) {
  return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
}

inline vec3 reflect(const vec3 &d, const vec3 &n) {
  return 2.f * n * dot(n, d) - d;
}

inline vec3 fresnel_schlick(vec3 r, double radians) {
  return r + (1.0 - r) * pow(1.0 - radians, 5.0);
}
inline double fresnel_schlick(double r, double radians) {
  return r + (1.0 - r) * pow(1.0 - radians, 5.0);
}

vec3 map_coords(vec3 axis, vec3 sample) {
  vec3 w = fabs(axis.x) > 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);
  vec3 u = normalize(cross(w, axis));
  vec3 v = cross(axis, u);
  return u * sample.x + v * sample.y + axis * sample.z;
}

double smith_ggx(vec3 normal, vec3 input, vec3 output, double alpha) {
  double cos_i = dot(normal, input);
  double cos_o = dot(normal, output);
  double a = cos_o * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_i * cos_i);
  double b = cos_i * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_o * cos_o);
  return 2.0 * cos_i * cos_o / (a + b);
}