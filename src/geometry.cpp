#include "geometry.h"
#include "camera.h"

#include <utility>

Triangle::Triangle(Vec3f v0, Vec3f v1, Vec3f v2)
        : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)) {
    normal = (v1 - v0).cross(v2 - v1).normalized();
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    float t = ((v0 - ray.origin).dot(normal)) / (ray.direction.dot(normal));
    if (t < 0) {
        return false;
    }
    Vec3f interactionPoint = ray.origin + t * ray.direction;
    Vec3f ivline1 = v0 - interactionPoint;
    Vec3f ivline2 = v1 - interactionPoint;
    Vec3f ivline3 = v2 - interactionPoint;
    float method_cross1 = ivline1.cross(ivline2).dot(normal);
    float method_cross2 = ivline2.cross(ivline3).dot(normal);
    float method_cross3 = ivline3.cross(ivline1).dot(normal);
    if ((method_cross1 * method_cross2 < 0) || (method_cross2 * method_cross3 < 0) ||
        (method_cross1 * method_cross3 < 0)) {
        //点不在三角形里面
        return false;
    }
    interaction.type = Interaction::GEOMETRY;
    interaction.pos = interactionPoint;
    //TODO uv here
//    int x=interactionPoint.x(),y=interactionPoint.y()
//    float alpha=(-(interactionPoint.x()-v1.x())*(v2.z()-v1.y())+(interactionPoint.y()-v1.y()))/();
    interaction.normal = normal;
    interaction.dist = t;
    if (material != nullptr) {
        interaction.model = material->evaluate(interaction);
    }
    return true;
}

Rectangle::Rectangle(Vec3f position, Vec2f dimension, Vec3f normal, Vec3f tangent)
        : Geometry(),
          position(std::move(position)),
          size(std::move(dimension)),
          normal(std::move(normal)),
          tangent(std::move(tangent)) {}
extern std::shared_ptr<Camera> camera;
bool Rectangle::intersect(Ray &ray, Interaction &interaction) const {
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
    if (sqrtf(delta.dot(delta) - touying * touying) > size[1] / 2) {
        return false;
    }
    interaction.dist = t * ray.direction.norm();

    interaction.pos = intersectPoint;
    interaction.normal = normal;
    interaction.type = Interaction::GEOMETRY;
    float offset_x=(intersectPoint-position).dot(tangent);
    float offset_y=(intersectPoint-position).dot(normal.cross(tangent));
    float u=offset_x/size(0)+0.5;
    float v=offset_y/size(1)+0.5;
    interaction.uv=Vec2f(u,v);
    interaction.tangent=tangent;
    auto uvd=camera->getdxdy(intersectPoint);
    interaction.dudv=sqrt(uvd(0)*uvd(0)+uvd(1)*uvd(1));
    if (material != nullptr) {
        interaction.model = material->evaluate(interaction);
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

Vec3f Rectangle::getPosition() const {
    return position;
}

Ellipsoid::Ellipsoid(const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c)
        : p(p), a(a), b(b), c(c) {}

bool Ellipsoid::intersect(Ray &ray, Interaction &interaction) const {
    using Eigen::Matrix4f;
    float a_length = a.norm();
    float b_length = b.norm();
    float c_length = c.norm();
    Matrix4f T{
            {1, 0, 0, p.x()},
            {0, 1, 0, p.y()},
            {0, 0, 1, p.z()},
            {0, 0, 0, 1}
    };
    Matrix4f R{
            {a.x() / a_length, b.x() / b_length, c.x() / c_length, 0},
            {a.y() / a_length, b.y() / b_length, c.y() / c_length, 0},
            {a.z() / a_length, b.z() / b_length, c.z() / c_length, 0},
            {0,                0,                0,                1}
    };
    Matrix4f S{
            {a_length, 0,        0,        0},
            {0,        b_length, 0,        0},
            {0,        0,        c_length, 0},
            {0,        0,        0,        1}
    };
    Matrix4f M = T * R * S;
    Matrix4f M_inverse = M.inverse();
    Vec3f ray_origin = (M_inverse * Vec4f(ray.origin.x(), ray.origin.y(), ray.origin.z(), 1)).head<3>();
    Vec3f ray_direction = (M_inverse * Vec4f(ray.direction.x(), ray.direction.y(), ray.direction.z(), 0)).head<3>();

    float circle_center_projection = ray_origin.dot(ray_direction.normalized());
    float projection = (ray_origin.dot(ray_origin) - circle_center_projection * circle_center_projection);
    if (projection > 1)
        return false;
    ////确认ray和单位圆相交
    float tv = sqrt(1 - projection) / ray_direction.norm();
    float t_center = -(circle_center_projection / ray_direction.norm());
    float t_enter = t_center - tv;
    float t_exit = t_center + tv;
    if (t_enter > 1e-4 || t_exit < ray.t_max) {
        t_enter = t_enter < 1e-4 ? 0 : t_enter;
        t_enter = t_enter == 0 ? t_exit : t_enter;
        //// initialize vec4
        Vec3f transformed_point = (ray_origin + t_enter * ray_direction);
        interaction.pos = (M * Vec4f(transformed_point.x(), transformed_point.y(), transformed_point.z(), 1)).head<3>();
        interaction.dist = t_enter;
        interaction.normal = (M * Vec4f(transformed_point.x(), transformed_point.y(), transformed_point.z(),
                                        0)).head<3>().normalized();
        interaction.type = Interaction::GEOMETRY;

        float theta = acosf(transformed_point.y())/PI;
        float phi = abs(atan2f(transformed_point.z(), transformed_point.x()))/PI;
        interaction.uv[0] = phi;
        interaction.uv[1] = theta;
        if (material != nullptr) {
            interaction.model = material->evaluate(interaction);
        }

        return true;
    }
    return false;
}
