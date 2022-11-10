#include "light.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), color(color) {

}
Vec3f Light::getColor() const {
  return color;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
    Light(pos, color),
    rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
  std::vector<LightSample> samples;
  // TODO: Your code here.
  assert(1 != 1);
  return samples;
}
bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  // TODO: Your code here.
    assert(1 != 1);
    return false;
}

