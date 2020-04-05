#pragma once

#include "vec3.h"

class Texture {
  public:
    Texture(float *data, int data_width, int data_height)
     : data(data), data_width(data_width), data_height(data_height) {}
    ~Texture() {
      delete data;
    }
    vec3 get_texture(double u, double v) const;
  private:
    float *data;
    int data_width, data_height;
};

vec3 Texture::get_texture(double u, double v) const {
  if (data == nullptr)
    return vec3(0,0,1);

  auto i = static_cast<int>(u * data_width);
  auto j = static_cast<int>((1 - v) * data_height - 0.001);

  if (i < 0) i = 0;
  if (j < 0) j = 0;
  if (i > data_width - 1)  i = data_width - 1;
  if (j > data_height - 1) j = data_height - 1;

  auto r = data[3 * i + 3 * data_width * j + 0];
  auto g = data[3 * i + 3 * data_width * j + 1];
  auto b = data[3 * i + 3 * data_width * j + 2];

  return vec3(r, g, b);
}