#pragma once

#include "Material.h"

class Lambert : public Material {
  public:
    Lambert(vec3 albedo) : albedo(albedo) {}
    vec3 sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
  private:
    vec3 albedo;
};

vec3 Lambert::sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  attenuation = albedo;
  return sample_cosine(rec->normal, seed);
}