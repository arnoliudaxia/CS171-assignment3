#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"

class Material {
 public:
  Material() = default;
  virtual ~Material() = default;
  [[nodiscard]] virtual InteractionPhongLightingModel evaluate(Interaction &interaction) const = 0;
};

class ConstColorMat : public Material {
 public:
  ConstColorMat();
  explicit ConstColorMat(const Vec3f &color, float sh = 16.f);
  ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);
  [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
 private:
  Vec3f diffuse_color;
  Vec3f specular_color;
  Vec3f ambient_color;
  float shininess;
};

class TextureMat : public Material {
public:
    class RGBColor{
    public:
        float R;
        float G;
        float B;
        RGBColor operator +(RGBColor other) const;
        RGBColor operator /(int value) const;
    };
    TextureMat()= default;
    explicit TextureMat(std::string texturepath);
    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;

protected:
    int texture_width;
    int texture_height;
//对于u,v上的点是rbgs[v][u]
std::vector<std::vector<RGBColor>> rgbs;
};

class MutliTextureMat : public TextureMat {
public:
    MutliTextureMat()= default;
    explicit MutliTextureMat(std::string texturepath,std::string normalpath);
    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
private:
    std::vector<std::vector<RGBColor>> normals;

};

class MipMapTextureMat : public TextureMat {
public:
    MipMapTextureMat()= default;
    explicit MipMapTextureMat(std::string texturepath);
    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
private:
    //第一层是level，第二层是u，第三层是v
    std::vector<std::vector<std::vector<RGBColor>>> mipmap;
    int level=0;
};
#endif //CS171_HW3_INCLUDE_MATERIAL_H_
