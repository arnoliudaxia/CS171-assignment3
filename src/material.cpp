#include "material.h"
#include "stb_image.h"

#include <utility>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
        : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
        : diffuse_color(std::move(diff)),
          specular_color(std::move(spec)),
          ambient_color(std::move(ambient)),
          shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
    InteractionPhongLightingModel m;
    m.diffusion = diffuse_color;
    m.specular = specular_color;
    m.shininess = shininess;
    return m;
}

TextureMat::TextureMat(std::string texturepath) {
    int w, h, n;
    unsigned char *raw_data = stbi_load(texturepath.c_str(), &w, &h, &n, 0);
    texture_width = w;
    texture_height = h;
    Vec3f rgb(0, 0, 0);
    int pixelCount = 0;
    for (int H = 0; H < h; ++H) {
        for (int W = 0; W < w; ++W) {
            rgb(0) = (float) raw_data[pixelCount++] / 255.0f;
            rgb(1) = (float) raw_data[pixelCount++] / 255.0f;
            rgb(2) = (float) raw_data[pixelCount++] / 255.0f;
            rgbs[h].push_back(rgb);
        }
    }
    stbi_image_free(raw_data);
}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
    //use u,v to render it
    //TODO 现在使用了nearest插值，可以考虑双线性插值
    int u = round(interaction.uv(0) * texture_width);
    int v = round(interaction.uv(0) * texture_width);
    Vec3f textureColor = rgbs[v][u];
    InteractionPhongLightingModel m;
    m.diffusion = textureColor;
    m.specular = textureColor;
    m.shininess = 32;
    return m;
}
