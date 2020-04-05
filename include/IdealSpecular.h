#pragma once

#include "Material.h"

class IdealSpecular : public Material {
  public:
    IdealSpecular() {}
    vec3 sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
};

vec3 IdealSpecular::sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  attenuation = vec3(0.9);
  return reflect(output, norm_dir);
}