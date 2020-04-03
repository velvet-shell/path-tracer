#pragma once

#include "vec3.h"
#include "utility.h"

class Material {
  public:
	  virtual ~Material() = default;
    virtual vec3 sample(vec3 output, vec3 normal, unsigned short *seed, vec3 &attenuation) const = 0;
};