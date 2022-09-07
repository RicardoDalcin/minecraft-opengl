#ifndef _CUBE_H
#define _CUBE_H

#include <vector>

#include "world/BlockDatabase.hpp"

struct CubeVertex
{
  glm::vec4 position;
  glm::vec2 textureCoords;
  glm::vec4 normal;
};

class Cube
{
private:
  Cube() {}
  ~Cube() {}

public:
  static std::vector<CubeVertex> GetVisibleVertices(int blockIndex, glm::vec3 position, std::array<glm::vec2, 36> vertices, std::array<bool, 6> occludedFaces);
};

#endif