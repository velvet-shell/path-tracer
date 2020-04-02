#pragma once

#include "Material.h"

class IdealSpecular : public Material {
  public:
    IdealSpecular() {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const override;
};

vec3 IdealSpecular::sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const {
  attenuation = vec3(1, 1, 1);
  return reflect(output, normal);
}