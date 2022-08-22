#ifndef _CUBE_H
#define _CUBE_H

#include "core/matrices.hpp"

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/VertexBufferLayout.hpp"

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
  glm::vec3 m_Position;
  const Texture *m_Texture;
  TextureInfo m_TextureReference;
  Shader *m_Shader;

  std::array<CubeVertex, 36> m_Vertices;

  VertexArray *m_VAO;
  VertexBuffer *m_VBO;

  const std::array<uint32_t, 36>
      m_Elements;
  const std::array<glm::vec4, 6> m_Normals;

public:
  Cube(glm::vec4 position, const Texture *texture, TextureInfo textureReference, Shader *shader);
  ~Cube();

  void Draw(glm::mat4 view, glm::mat4 projection);
};

#endif