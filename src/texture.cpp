#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.h"

void Texture::loadTextureFromFile(const std::string &path) {
  int w, h, ch;
  stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
  tex_data.assign(img, img + w * h * ch);
  stbi_image_free(img);
}