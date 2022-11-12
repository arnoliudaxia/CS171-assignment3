#include "light.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
        position(pos), color(color) {

}

Vec3f Light::getColor() const {
    return color;
}

Vec3f Light::getPosition() const {
    return position;
}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension,
                                 const Vec3f &normal,
                                 const Vec3f &tangent) :
        Light(pos, color),
        rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
    std::vector<LightSample> samples;
    const int SAMPLES = 8; //请确保这是个偶数
    Vec2f step=rectangle.getSize()/SAMPLES;
    Vec3f right = rectangle.getTangent();
    Vec3f up = right.cross(rectangle.getNormal());
    for (int i = 0; i < SAMPLES; i++) {
        for (int j = 0; j < SAMPLES; j++) {
            Vec3f pos = position + (i - SAMPLES / 2) * step(0) * right +
                        (j - SAMPLES / 2) * step(1) * up;
            LightSample ls;
            ls.position = pos;
            ls.color = color;
            samples.push_back(ls);

        }
    }
    return samples;
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    if (rectangle.intersect(ray, interaction)) {
        interaction.type = Interaction::LIGHT;
        return true;
    }
    return false;
}

