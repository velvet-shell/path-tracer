#pragma once

#include "Material.h"

class CookTorrance : public Material {
  public:
    CookTorrance(vec3 ks, double roughness)
     : ks(ks), roughness(roughness) {}
    vec3 sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
  private:
    vec3 ks;
    double roughness;
};

vec3 CookTorrance::sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  vec3 normal = norm_dir;
  vec3 half = sample_ggx(normal, roughness, seed);
  vec3 input = reflect(output, half);
  if (dot(normal, input) > 0.0 && dot(input, half) > 0.0) {
    vec3 fresnel = fresnel_schlick(ks, dot(half, input));
    double geometry = smith_ggx(normal, input, output, roughness);
    double weight = fabs(dot(output, half)) / (dot(normal, output) * dot(normal, half));
    attenuation = fresnel * geometry * weight;
  } else {
    attenuation = vec3();
  }
  return input;
}