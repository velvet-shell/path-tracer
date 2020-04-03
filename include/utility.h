#pragma once

#include "vec3.h"

inline double clamp(double x) {
  return x < 0 ? 0 : x > 1 ? 1 : x;
}

inline vec3 reflect(const vec3 &d, const vec3 &n) {
  return 2.f * n * dot(n, d) - d;
}

inline vec3 fresnel_schlick(vec3 r, double radians) {
  return r + (1.0 - r) * pow(1.0 - radians, 5.0);
}

vec3 map_coords(vec3 axis, vec3 sample) {
  vec3 w = fabs(axis.x) > 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);
  vec3 u = normalize(cross(w, axis));
  vec3 v = cross(axis, u);
  return u * sample.x + v * sample.y + axis * sample.z;
}

vec3 sample_cosine(vec3 axis, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = sqrt(1 - r2);
  vec3 input(h * cos(theta), h * sin(theta), sqrt(r2));
  return map_coords(axis, input);
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

vec3 sample_ggx(vec3 axis, double alpha, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = acos(sqrt((1.0 - r1) / (r1 * (alpha * alpha - 1.0) + 1.0)));
  double phi = 2.0 * M_PI * r2;
  vec3 half(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  return map_coords(axis, half);
}

double smith_ggx(vec3 normal, vec3 input, vec3 output, double alpha) {
  double cos_i = dot(normal, input);
  double cos_o = dot(normal, output);
  double a = cos_o * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_i * cos_i);
  double b = cos_i * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_o * cos_o);
  return 2.0 * cos_i * cos_o / (a + b);
}