#pragma once

#include <cmath>

class vec3 {
  public:
    vec3() : x(0), y(0), z(0) {}
    vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    
    vec3 operator-() const {
      return vec3(-x, -y, -z);
    }
    vec3& operator+=(const vec3 &v) {
      x += v.x;
      y += v.y;
      z += v.z;
    return *this;
    }
    vec3& operator*=(const double t) {
      x *= t;
      y *= t;
      z *= t;
    return *this;
    }
    vec3& operator/=(const double t) {
    return *this *= 1 / t;
    }
    double norm() const {
      return sqrt(x * x + y * y + z * z);
    }

    double x, y, z;
};

inline vec3 operator+(const vec3 &u, const vec3 &v) {
  return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
inline vec3 operator-(const vec3 &u, const vec3 &v) {
  return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}
inline vec3 operator*(const vec3 &u, const vec3 &v) {
  return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}
inline vec3 operator*(double t, const vec3 &u) {
  return vec3(t * u.x, t * u.y, t * u.z);
}
inline vec3 operator*(const vec3 &u, double t) {
  return t * u;
}
inline vec3 operator+(double t, const vec3 &u) {
  return vec3(t + u.x, t + u.y, t + u.z);
}
inline vec3 operator+(const vec3 &u, double t) {
  return t + u;
}
inline vec3 operator-(double t, const vec3 &u) {
  return vec3(t - u.x, t - u.y, t - u.z);
}
inline vec3 operator-(const vec3 &u, double t) {
  return t - u;
}
inline vec3 operator/(const vec3 &u, double t) {
  return (1 / t) * u;
}
inline double dot(const vec3 &u, const vec3 &v) {
  return u.x * v.x +
         u.y * v.y +
         u.z * v.z;
}
inline vec3 cross(const vec3 &u, const vec3 &v) {
  return vec3(u.y * v.z - u.z * v.y,
              u.z * v.x - u.x * v.z,
              u.x * v.y - u.y * v.x);  
}
inline vec3 normalize(const vec3 &u) {
  return u / u.norm();
}
inline double max(const vec3 &u) {
  return std::max(u.x, std::max(u.y, u.z));
}
vec3 map_coords(vec3 axis, vec3 sample) {
  vec3 w = fabs(axis.x) > 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);
  vec3 u = normalize(cross(w, axis));
  vec3 v = cross(axis, u);
  return u * sample.x + v * sample.y + axis * sample.z;
}
vec3 sample_cosine_powered(vec3 axis, double power, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = pow(r2, 1.0 / (power + 1.0));
  double sin_phi = sqrt(1 - pow(r2, 2.0 / (power + 1.0)));
  vec3 input(sin_phi * cos(theta), sin_phi * sin(theta), h);
  return map_coords(axis, input);
}
vec3 sample_cosine(vec3 axis, unsigned short *seed) {
  double r1 = erand48(seed);
  double r2 = erand48(seed);
  double theta = 2 * M_PI * r1;
  double h = sqrt(1 - r2);
  vec3 input(h * cos(theta), h * sin(theta), sqrt(r2));
  return map_coords(axis, input);
}
vec3 schlick_approximation(vec3 r, vec3 input, vec3 normal) {
  return r + (1.0 - r) * pow((1 - dot(input, normal)), 5);
}
inline vec3 reflect(const vec3 &d, const vec3 &n) {
  return 2.f * n * dot(n, d) - d;
}