#pragma once

#include "Material.h"

class IdealSpecular : public Material {
  public:
    IdealSpecular() {}
    vec3 sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
};

vec3 IdealSpecular::sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  attenuation = vec3(1.0, 1.0, 1.0);
  return reflect(output, rec->normal);
}