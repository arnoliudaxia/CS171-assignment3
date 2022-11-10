#include "geometry.h"

#include <utility>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
    : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
  normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
    assert(1 != 1);
  return false;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
    : Geometry(),
      position(std::move(position)),
      size(std::move(dimension)),
      normal(std::move(normal)),
      tangent(std::move(tangent)) {}

bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: 需要测试
    //ray上的点和中心的连线和法线点积为0
    float t = (normal.dot(position - ray.origin)) / (ray.direction.dot(normal));
    if (t < 0) {
        return false;
    }
    Vec3f intersectPoint = ray.origin + t * ray.direction;
    Vec3f delta = intersectPoint - position;
    float touying = abs(delta.dot(tangent));
    if (touying > size[0] / 2) {
        return false;
    }
    if (sqrtf(delta.dot(delta) - touying* touying)>size[1]/2)
    {
        return false;
    }
    return true;
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
    using Eigen::Matrix4f;
    Matrix4f T{
        {1,0,0,c.x()},
        {0,1,0,c.y()},
        {0,0,1,c.z()},
        {0,0,0,1}
    };
    Vec3f
    return false;
}
