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
public:
  static const int CHUNK_SIZE = 16;
  static const int CHUNK_HEIGHT = 64;

private:
  VertexArray *m_VAO;
  VertexBuffer *m_VBO;

  int m_MeshVertexCount;

  Shader *m_Shader;
  Texture *m_TextureAtlas;
  std::array<std::array<std::array<Cube *, CHUNK_SIZE>, CHUNK_HEIGHT>, CHUNK_SIZE> m_Cubes;

public:
  Chunk(Shader *shader);
  ~Chunk();

  void BuildMesh();

  void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
};

#endif