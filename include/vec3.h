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
  return -(t - u);
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
inline double mean(const vec3 &u) {
  return (u.x + u.y + u.z) / 3;
}