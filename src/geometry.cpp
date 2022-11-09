#include "geometry.h"

#include <utility>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
  return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here
  return false;
}
Vec2f Rectangle::getSize() const {
  return size;
}
Vec3f Rectangle::getNormal() const {
  return normal;
}
Vec3f Rectangle::getTangent() const {
  return tangent;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
    : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
  return false;
}
