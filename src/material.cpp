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
    int pixelCount = 0;
    for (int H = 0; H < h; ++H) {
        rgbs.emplace_back(w);
        for (int W = 0; W < w; ++W) {
            RGBColor rgbColor;
            rgbColor.R = (float) raw_data[pixelCount++] / 255.0f;
            rgbColor.G = (float) raw_data[pixelCount++] / 255.0f;
            rgbColor.B = (float) raw_data[pixelCount++] / 255.0f;
            rgbs[H][W]=rgbColor;
        }
    }
    stbi_image_free(raw_data);
}

InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
    //use u,v to render it
    //TODO 现在使用了nearest插值，可以考虑双线性插值
    int u = round(interaction.uv(0) * (texture_width-1));
    int v = round(interaction.uv(1) * (texture_height-1));
    auto textureColor = rgbs[v][u];
    Vec3f color(textureColor.R,textureColor.G,textureColor.B);
    InteractionPhongLightingModel m;
    m.diffusion = color;
    m.specular = color;
    m.shininess = 32;
    return m;
}
