#include "integrator.h"
#include <omp.h>

#include <utility>

PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam,
                                                 std::shared_ptr<Scene> scene)
    : camera(std::move(cam)), scene(std::move(scene)) {
}

void PhongLightingIntegrator::render() const {
  Vec2i resolution = camera->getImage()->getResolution();
  int cnt = 0;
#pragma omp parallel for schedule(guided, 2), shared(cnt)
  for (int dx = 0; dx < resolution.x(); dx++) {
#pragma omp atomic
    ++cnt;
    printf("\r%.02f%%", cnt * 100.0 / resolution.x());
    for (int dy = 0; dy < resolution.y(); dy++) {
      Vec3f L(0, 0, 0);
      // TODO: Your code here.
      camera->getImage()->setPixel(dx, dy, L);
    }
  }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
  Vec3f radiance(0, 0, 0);
  // TODO: Your code here.
  return radiance;
}
