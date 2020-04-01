#pragma once

#include "Material.h"

class IdealSpecular : public Material {
  public:
    IdealSpecular() {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed) const override;
    double pdf(vec3 input, vec3 output, vec3 normal) const override;
    vec3 brdf(vec3 input, vec3 output, vec3 normal) const override;
    vec3 eval(vec3 input, vec3 output, vec3 normal) const override;
};

vec3 IdealSpecular::sample(vec3 output, vec3 normal, unsigned short *seed) const {
  return reflect(output, normal);
}

double IdealSpecular::pdf(vec3 input, vec3 output, vec3 normal) const {
  return 1;
}

vec3 IdealSpecular::brdf(vec3 input, vec3 output, vec3 normal) const {
  return vec3(1, 1, 1) / dot(input, normal);
}

vec3 IdealSpecular::eval(vec3 input, vec3 output, vec3 normal) const {
  return vec3(1, 1, 1);
}