#ifndef _CHUNK_H
#define _CHUNK_H

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "world/Cube.hpp"

class Chunk
{
private:
  static const int CHUNK_SIZE = 16;
  static const int CHUNK_HEIGHT = 4;

  Shader *m_Shader;
  Texture *m_TextureAtlas;
  std::array<Cube *, CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT> m_Cubes;

public:
  Chunk(Shader *shader);
  ~Chunk();

  void Draw(glm::mat4 view, glm::mat4 projection);
};

#endif