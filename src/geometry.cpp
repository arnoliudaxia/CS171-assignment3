#include "geometry.h"

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
    Vec3f 交顶连线1 = v0 - interactionPoint;
    Vec3f 交顶连线2 = v1 - interactionPoint;
    Vec3f 交顶连线3 = v2 - interactionPoint;
    float 方法叉积1 = 交顶连线1.cross(交顶连线2).dot(normal);
    float 方法叉积2 = 交顶连线2.cross(交顶连线3).dot(normal);
    float 方法叉积3 = 交顶连线3.cross(交顶连线1).dot(normal);
    if ((方法叉积1 * 方法叉积2 < 0) || (方法叉积2 * 方法叉积3 < 0) || (方法叉积1 * 方法叉积3 < 0)) {
        //点不在三角形里面
        return false;
    }
    interaction.type = Interaction::GEOMETRY;
    interaction.pos = interactionPoint;
    //TODO uv here
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
    if (material != nullptr) {
        interaction.model = material->evaluate(interaction);
    }
    interaction.pos = intersectPoint;
    interaction.normal = normal;
    interaction.type = Interaction::GEOMETRY;
    //TODO texture uv
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

    float 圆心投影 = ray_origin.dot(ray_direction.normalized());
    float 投影 = (ray_origin.dot(ray_origin) - 圆心投影 * 圆心投影);
    if (投影 > 1)
        return false;
    ////确认ray和单位圆相交
    float tv = sqrt(1 - 投影) / ray_direction.norm();
    float t_center = -(圆心投影 / ray_direction.norm());
    float t_enter = t_center - tv;
    float t_exit = t_center + tv;
    if (t_enter > 1e-4 || t_exit < ray.t_max) {
        t_enter = t_enter < 1e-4 ? 0 : t_enter;
        t_enter=t_enter==0?t_exit:t_enter;
        //// initialize vec4
        Vec3f 变换碰撞点 = (ray_origin + t_enter * ray_direction);
        interaction.pos = (M * Vec4f(变换碰撞点.x(), 变换碰撞点.y(), 变换碰撞点.z(), 1)).head<3>();
        interaction.dist = t_enter;
        interaction.normal = (M * Vec4f(变换碰撞点.x(), 变换碰撞点.y(), 变换碰撞点.z(), 0)).head<3>().normalized();
        interaction.type = Interaction::GEOMETRY;

//TODO 为什么加了UV会错
        //        float theta = acosf(变换碰撞点.y());
//        float phi = abs(atan2f(变换碰撞点.z(), 变换碰撞点.x()));
//        interaction.uv[0] = phi;
//        interaction.uv[1] = theta;
        if (material != nullptr) {
            interaction.model = material->evaluate(interaction);
        }

        return true;
    }
    return false;
}
