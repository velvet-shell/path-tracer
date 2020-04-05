#pragma once

#include "Material.h"
#include "Texture.h"

class Lambert : public Material {
  public:
    Lambert(vec3 albedo) : albedo(albedo) {}
    Lambert(const Texture *texture) : texture(texture) {}
    
    vec3 sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const override;
  private:
    vec3 albedo;
    const Texture *texture = nullptr;
};

vec3 Lambert::sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const {
  if (texture) {
    attenuation = texture->get_texture(rec->u, rec->v);
  } else {
    attenuation = albedo;
  }
  return sample_cosine(rec->normal, seed);
}