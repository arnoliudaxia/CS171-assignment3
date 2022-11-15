#include "camera.h"

Camera::Camera()
        : position(0, -1, 0), fov(45), focal_len(1) {
    lookAt({0, 0, 0}, {0, 0, 1});
}

Ray Camera::generateRay(float dx, float dy) {
    int resolution_x = resolution[0];
    int resolution_y = resolution[1];
    dx = fmax(0, dx);
    dy = fmax(0, dy);
    dx = fmin(dx, resolution_x);
    dy = fmin(dy, resolution_y);

    Vec2i deltaPixel = Vec2i(resolution_x / 2, resolution_y / 2) - Vec2i(dx, dy);
    Vec3f image_plane_center = position + forward;
    Vec3f pointPixel =
            image_plane_center * focal_len + right * deltaPixel[0] * dx_perPixel - up * deltaPixel[1] * dy_perPixel;

    return Ray(position, (pointPixel - position).normalized());
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    forward = (look_at - position).normalized();
    right = ref_up.cross(forward).normalized();
    up = ref_up;
}

void Camera::setPosition(const Vec3f &pos) {
    position = pos;
}

Vec3f Camera::getPosition() const {
    return position;
}

void Camera::setFov(float new_fov) {
    fov = new_fov;
}

float Camera::getFov() const {
    return fov;
}

/// 除了设置分辨率外，算好在image plane上临近像素的距离
/// \param img
void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
    image = img;
    resolution = image->getResolution();
    int resolution_x = resolution[0];
    int resolution_y = resolution[1];
    float YLength = focal_len * tanf(fov / 180.0 * PI / 2) * 2;
    float XLength = YLength * resolution_x / resolution_y;
     dx_perPixel = XLength / resolution_x;
     dy_perPixel = YLength / resolution_y;
}

std::shared_ptr<ImageRGB> &Camera::getImage() {
    return image;
}

Vec2f Camera::getdxdy(Vec3f positionInWorld) {
    float distance=(position-positionInWorld).norm();
    return Vec2f(dx_perPixel*distance,dy_perPixel*distance);
}

