#pragma once

#include "vec3.h"
#include "Ray.h"

class Camera {
  public:
    Camera(vec3 origin, vec3 lookat, vec3 up, double fov, double aspect) : origin(origin)  {
      double half_height = tan(fov / 2);
      double half_width = aspect * half_height;

      vec3 w = normalize(origin - lookat);
      vec3 u = normalize(cross(up, w));
      vec3 v = cross(w, u);

      lower_left = origin - half_width * u - half_height * v - w;

      horizontal = 2 * half_width * u;
      vertical = 2 * half_height * v;
    }
    Ray get_ray(double x, double y) {
      return Ray(origin, normalize(lower_left + x * horizontal + y * vertical - origin));
    }
  public:
    vec3 origin;
    vec3 lower_left;
    vec3 horizontal;
    vec3 vertical;
};