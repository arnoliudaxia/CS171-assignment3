#include "material.h"
#include "stb_image.h"
#include "camera.h"

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

MutliTextureMat::MutliTextureMat(std::string texturepath,std::string normalpath) : TextureMat(texturepath) {
    int w, h, n;
    unsigned char *raw_data = stbi_load(normalpath.c_str(), &w, &h, &n, 0);
    int pixelCount = 0;
    for (int H = 0; H < h; ++H) {
        normals.emplace_back(w);
        for (int W = 0; W < w; ++W) {
            RGBColor rgbColor{};
            rgbColor.R = (float) raw_data[pixelCount++] / 255.0f;
            rgbColor.G = (float) raw_data[pixelCount++] / 255.0f;
            rgbColor.B = (float) raw_data[pixelCount++] / 255.0f;
            normals[H][W]=rgbColor;
        }
    }
    stbi_image_free(raw_data);
}

InteractionPhongLightingModel MutliTextureMat::evaluate(Interaction &interaction) const {
    auto phongmodel=TextureMat::evaluate(interaction);
    //TODO 现在使用了nearest插值，可以考虑双线性插值
    int u = round(interaction.uv(0) * (texture_width-1));
    int v = round(interaction.uv(1) * (texture_height-1));
    RGBColor nr=normals[v][u];
    nr.R=nr.R*2-1;
    nr.G=nr.G*2-1;
    nr.B=nr.B*2-1;
    interaction.normal=(interaction.normal*nr.B+interaction.tangent*nr.R+interaction.tangent.cross(interaction.normal)*nr.G).normalized();
    return phongmodel;
}


MipMapTextureMat::MipMapTextureMat(std::string texturepath) : TextureMat(texturepath) {
    level=(int) log2(texture_width);
//    mipmap.emplace_back((int) log2(texture_width));
    mipmap.push_back(rgbs);
    for (int i = 1; i < level; ++i) {
        //生成第i层
        std::vector<std::vector<RGBColor>> levelrgbs;
        int levelWidth=mipmap[i-1].size();
        levelWidth=levelWidth-levelWidth%2;
        for (int y = 0; y < levelWidth-1; ++++y) {
        levelrgbs.emplace_back(levelWidth/2);
            for (int x = 0; x < levelWidth-1; ++++x) {
                levelrgbs[y/2][x/2]=(mipmap[i-1][y][x]+mipmap[i-1][y][x+1]+mipmap[i-1][y+1][x]+mipmap[i-1][y+1][x+1])/4;
//                levelrgbs[y/2][x/2]=(mipmap[i-1][y][x]);
            }
        }
        mipmap.push_back(std::move(levelrgbs));
    }

}

InteractionPhongLightingModel MipMapTextureMat::evaluate(Interaction &interaction) const {
//    auto phongmodel=TextureMat::evaluate(interaction);
    //现在考虑取第几曾
    float L=interaction.dudv*104;
//    printf("%f.04\n",L);
    int D= (int)log2(L);
    if(D>mipmap.size()-1)
    {
        D=mipmap.size()-1;
    }

    int u = round(interaction.uv(0) * (texture_width-1));
    int v = round(interaction.uv(1) * (texture_height-1));
    auto textureColor = mipmap[D][v/(pow(2,D))][u/(pow(2,D))];
//    auto textureColor = mipmap[0][v/2][u/2];
    Vec3f color(textureColor.R,textureColor.G,textureColor.B);
    InteractionPhongLightingModel m;
    m.diffusion = color;
    m.specular = color;
    m.shininess = 32;
    return m;
}

TextureMat::RGBColor TextureMat::RGBColor::operator+(TextureMat::RGBColor other) const {
    RGBColor result;
    result.R=this->R+other.R;
    result.G=this->G+other.G;
    result.B=this->B+other.B;
    return result;
}

TextureMat::RGBColor TextureMat::RGBColor::operator/(int value) const {
    TextureMat::RGBColor result;
    result.R=this->R/value;
    result.G=this->G/value;
    result.B=this->B/value;
    return result;
}
