#ifndef _CHUNK_H
#define _CHUNK_H

#include <vector>

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "world/Cube.hpp"

class Chunk
{
private:
  static const int CHUNK_SIZE = 16;
  static const int CHUNK_HEIGHT = 4;

  VertexArray *m_VAO;
  VertexBuffer *m_VBO;

  std::vector<CubeVertex> m_Vertices;

  Shader *m_Shader;
  Texture *m_TextureAtlas;
  std::array<std::array<std::array<Cube *, CHUNK_SIZE>, CHUNK_HEIGHT>, CHUNK_SIZE> m_Cubes;

public:
  Chunk(Shader *shader);
  ~Chunk();

  void Draw(glm::mat4 view, glm::mat4 projection);
};

#endif