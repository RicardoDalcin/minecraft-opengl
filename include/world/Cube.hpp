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
  TextureInfo m_TextureReference;
  std::array<glm::vec2, 36> m_TextureCoords;

  std::array<CubeVertex, 36> m_Vertices;

  // VertexArray *m_VAO;
  // VertexBuffer *m_VBO;

  // const std::array<uint32_t, 36>
  //     m_Elements;
  // const std::array<glm::vec4, 6> m_Normals;

public:
  Cube(glm::vec4 position, TextureInfo textureReference, std::array<glm::vec2, 36> textureCoords);
  ~Cube();

  glm::vec3 GetPosition() const { return m_Position; }
  std::array<CubeVertex, 36> GetVertices() const { return m_Vertices; }

  // void Draw(glm::mat4 view, glm::mat4 projection);

  std::vector<CubeVertex> GetVisibleVertices(std::array<bool, 6> occludedFaces);

  static std::vector<CubeVertex> GetVisibleVertices(int blockIndex, glm::vec3 position, std::array<glm::vec2, 36> vertices, std::array<bool, 6> occludedFaces);
};

#endif