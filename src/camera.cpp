#include "camera.h"

Camera::Camera()
    : position(0, -1, 0), fov(45), focal_len(1) {
  lookAt({0, 0, 0}, {0, 0, 1});
}

Ray Camera::generateRay(float dx, float dy) {
  // TODO: Your code here
  // You need to generate ray according to screen coordinate (dx, dy)
  return Ray(Vec3f(0,0,0), Vec3f(0,0,0));
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
  // TODO: Your code here
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

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
  image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
  return image;
}

