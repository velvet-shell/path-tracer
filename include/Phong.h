#pragma once

#include "Material.h"

class Phong : public Material {
  public:
    Phong(vec3 kd, vec3 ks, double exponent)
     : kd(kd), ks(ks), exponent(exponent) {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const override;
  private:
    vec3 kd;
    vec3 ks;
    double exponent;
};

vec3 Phong::sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const {
  double pd = max(kd);
  double ps = max(ks);
  double p = pd / (pd + ps);
  double ksi = erand48(seed) * (pd + ps);
  vec3 input;
  if (ksi < p) {
    //sample lambert
    input = sample_cosine(normal, seed);
    attenuation = kd / p;
  } else {
    //sample phong
    input = sample_cosine_powered(reflect(output, normal), exponent, seed);
    if (dot(input, normal) < 0) {
      attenuation = vec3();
    } else {
      attenuation = (exponent + 2) / (exponent + 1) * dot(input, normal) * ks / (1 - p);
    }
  }
  return input;
}