#pragma once

#include "vec3.h"

class Material {
  public:
	  virtual ~Material() = default;
    virtual vec3 sample(vec3 output, vec3 normal, unsigned short *seed) const = 0;
    virtual vec3 eval(vec3 input, vec3 output, vec3 normal) const = 0;
};