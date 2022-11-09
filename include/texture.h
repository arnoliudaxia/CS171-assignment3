#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

class Texture {
 public:
  enum Type { DIFF, NORM, DISP };
  Type type;
  void loadTextureFromFile(const std::string &path);

 private:
  std::vector<uint8_t> tex_data;
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
