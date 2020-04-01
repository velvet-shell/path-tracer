#pragma once

#include "Material.h"

class IdealSpecular : public Material {
  public:
    IdealSpecular() {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed) const override;
    vec3 eval(vec3 input, vec3 output, vec3 normal) const override;
};

vec3 IdealSpecular::sample(vec3 output, vec3 normal, unsigned short *seed) const {
  return reflect(output, normal);
}

vec3 IdealSpecular::eval(vec3 input, vec3 output, vec3 normal) const {
  return vec3(1, 1, 1);
}