#pragma once

#include "Material.h"

class ModifiedPhong : public Material {
  public:
    ModifiedPhong(vec3 kd, vec3 ks, double exponent)
     : kd(kd), ks(ks), exponent(exponent) {}
    vec3 sample(vec3 output, vec3 normal, unsigned short *seed) const override;
    vec3 eval(vec3 input, vec3 output, vec3 normal) const override;
  private:
    vec3 kd;
    vec3 ks;
    double exponent;
};

vec3 ModifiedPhong::sample(vec3 output, vec3 normal, unsigned short *seed) const {
  double pd = max(kd);
  double ps = max(ks);
  double ksi = erand48(seed) * (pd + ps);
  double p = pd / (pd + ps);

  vec3 input;
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;

  if (ksi < p) {
    //sample lambert
    double h = sqrt(1 - r2);
    input = vec3(h * cos(theta), h * sin(theta), sqrt(r2));
    input = map_coords(normal, input);
  } else {
    //sample phong
    double h = pow(r2, 1 / (exponent + 1));
    double sin_phi = sqrt(1 - h * h);
    input = vec3(sin_phi * cos(theta), sin_phi * sin(theta), h);
    input = map_coords(reflect(output, normal), input);
  }
  return input;
}

vec3 ModifiedPhong::eval(vec3 input, vec3 output, vec3 normal) const {
  double p = max(kd) / (max(kd) + max(ks));
  double s = dot(reflect(input, normal), output);
  if (s < 0.0) {
    s = 0.0;
  } else {
    s = (exponent + 2) * pow(s, exponent) / 2;
  }
  double cosi = dot(input, normal);
  return (kd + ks * s) * cosi / (p * cosi + (1 - p) * s);
}