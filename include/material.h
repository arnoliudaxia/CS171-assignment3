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
    };
    TextureMat()= default;
    explicit TextureMat(std::string texturepath);
    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
private:
    //对于u,v上的点是rbgs[v][u]
    std::vector<std::vector<RGBColor>> rgbs;
protected:
    int texture_width;
    int texture_height;
};

class MutliTextureMat : public TextureMat {
public:
    MutliTextureMat()= default;
    explicit MutliTextureMat(std::string texturepath,std::string normalpath);
    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;
private:
    std::vector<std::vector<RGBColor>> normals;

};
#endif //CS171_HW3_INCLUDE_MATERIAL_H_
