#pragma once

#include "utility.h"

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

double triangle_distribution(unsigned short *seed) {
  double r = 2 * erand48(seed);
  if (r < 1) {
    return sqrt(r) - 1;
  } else {
    return 1 - sqrt(2 - r);
  }
}