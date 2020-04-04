#pragma once

#include "vec3.h"
#include "utility.h"
#include "sample.h"
#include "Hittable.h"

class Material {
  public:
	  virtual ~Material() = default;
    virtual vec3 sample(vec3 output, const hit_record *rec, unsigned short *seed, vec3 &attenuation) const = 0;
};