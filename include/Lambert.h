#pragma once

#include "Material.h"

class Lambert : public Material {
  public:
    Lambert(vec3 albedo) : albedo(albedo) {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed) const override;
    double pdf(vec3 input, vec3 output, vec3 normal) const override;
    vec3 brdf(vec3 input, vec3 output, vec3 normal) const override;
    vec3 eval(vec3 input, vec3 output, vec3 normal) const override;
  private:
    vec3 albedo;
};

vec3 Lambert::sample(vec3 output, vec3 normal, unsigned short *seed) const {
  // cosine distribution
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = sqrt(1 - r2);
  vec3 input(h * cos(theta), h * sin(theta), sqrt(r2));
  input = map_coords(normal, input);
  return input;
}

double Lambert::pdf(vec3 input, vec3 output, vec3 normal) const {
  return dot(input, normal) * M_1_PI;
}

vec3 Lambert::brdf(vec3 input, vec3 output, vec3 normal) const {
  return albedo * M_1_PI;
}

vec3 Lambert::eval(vec3 input, vec3 output, vec3 normal) const {
  return albedo;
}