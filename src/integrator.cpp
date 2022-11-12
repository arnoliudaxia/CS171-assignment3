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
            Ray ray = camera->generateRay(dx, dy);
            Interaction interaction;
            if (scene->intersect(ray, interaction)) {
                L = radiance(ray, interaction);
            }
            camera->getImage()->setPixel(dx, dy, L);
        }
    }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
    auto light = scene->getLight();
    auto lightColor = light->getColor();
    if (interaction.type == Interaction::LIGHT) {
        return lightColor;
    }
    //先获取ambient
    Vec3f ambient = (scene->getAmbient()).cwiseProduct(interaction.model.diffusion);
    //算diffuse
    auto lightSamples = scene->getLight()->samples();
    Vec3f diff(0,0,0);
    Vec3f spec(0,0,0);
    for (LightSample sample: lightSamples) {
        Vec3f 点到光点=sample.position - interaction.pos;
        Ray shadow_ray(interaction.pos, (点到光点).normalized()
        ,RAY_DEFAULT_MIN,点到光点.norm());
        if (!scene->isShadowed(shadow_ray)) {
            //计算Diffuse
            Vec3f lightDir = (sample.position - interaction.pos).normalized();
            float diff_weight = fmax(interaction.normal.dot(lightDir), 0.0);
            diff += diff_weight*interaction.model.diffusion;
            //计算Specular
            Vec3f viewDir = (ray.origin - interaction.pos).normalized();
            Vec3f cp=interaction.pos+interaction.normal.normalized();
            float costheta=lightDir.dot(interaction.normal)/(lightDir.norm()*interaction.normal.norm());
            Vec3f lp=interaction.pos+lightDir.normalized()*1/costheta;
            Vec3f rp=2*cp-lp;
            Vec3f reflectDir = (rp-interaction.pos).normalized();
            float spec_weight = pow(fmaxf(viewDir.dot(reflectDir), 0.0), interaction.model.shininess);
            spec += spec_weight*interaction.model.specular;
        }
    }
    diff /= lightSamples.size();
    spec /= lightSamples.size();
//    Vec3f lightDir = normalize( - FragPos);
    Vec3f result=(ambient+diff+spec).cwiseProduct(lightColor);
    return result;
}
