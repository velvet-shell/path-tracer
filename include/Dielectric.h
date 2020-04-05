#pragma once

#include "Material.h"

class Dielectric : public Material {
  public:
    Dielectric() {}
    vec3 sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
  private:
    double refractive_index = 1.5;
};

vec3 Dielectric::sample(vec3 output, vec3 norm_dir, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  double cosi = dot(output, rec->normal);
  double eta = 1.0 / refractive_index;
  if (cosi < 0) {
    cosi = -cosi;
    eta = refractive_index;
  }
  double cost2 = 1.0 - eta * eta * (1.0 - cosi * cosi);
  if (cost2 < 0) {
    attenuation = vec3(0.9);
    return reflect(output, rec->normal);
  }
  double F0 = (eta - 1.0) * (eta - 1.0) / ((eta + 1.0) * (eta + 1.0));
  double fresnel = fresnel_schlick(F0, cosi);
  double P = 0.25 + 0.5 * fresnel;
  if (erand48(seed) < P) {
    attenuation = vec3(fresnel / P);
    return reflect(output, rec->normal);
  } else {
    attenuation = vec3((1.0 - fresnel) / (1.0 - P));
    return -output * eta + norm_dir * (eta * cosi - sqrt(cost2));
  }
}