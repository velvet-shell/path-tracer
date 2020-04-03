#pragma once

#include "Material.h"

class CookTorrance : public Material {
  public:
    CookTorrance(vec3 ks, double roughness)
     : ks(ks), roughness(roughness) {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const override;
  private:
    vec3 ks;
    double roughness;
};

vec3 sample_ggx(vec3 output, double alpha, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = acos(sqrt((1.0 - r1) / (r1 * (alpha * alpha - 1.0) + 1.0)));
  double phi = 2.0 * M_PI * r2;
  vec3 input(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  return input;
}

double smith_ggx(vec3 normal, vec3 input, vec3 output, double alpha) {
  double cos_i = dot(normal, input);
  double cos_o = dot(normal, output);
  double a = cos_o * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_i * cos_i);
  double b = cos_i * sqrt(alpha * alpha + (1.0 - alpha * alpha) * cos_o * cos_o);
  return 2.0 * cos_i * cos_o / (a + b);
}

double ggx_ndf(vec3 normal, vec3 half, double alpha) {
  double cos_theta = dot(normal, half);
  double denominator = (alpha * alpha - 1.0) * cos_theta * cos_theta + 1.0;
  return alpha * alpha / (M_PI * denominator * denominator);
}

vec3 CookTorrance::sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const {
  vec3 half = sample_ggx(output, roughness, seed);
  vec3 input = normalize(reflect(output, half));
  if (dot(normal, input) > 0.0 && dot(input, half) > 0.0) {
    vec3 fresnel = schlick_approximation(ks, dot(half, input));
    double geometry = smith_ggx(normal, input, output, roughness);
    attenuation = fresnel * geometry * fabs(dot(output, half)) / (dot(normal, output) * dot(normal, half));
  } else {
    attenuation = vec3();
  }
  return input;
}