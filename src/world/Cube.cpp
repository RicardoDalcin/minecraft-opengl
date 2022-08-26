#include <glad/glad.h>

#include "world/Cube.hpp"

static const std::array<uint32_t, 36> ELEMENTS = {
    // Each set of 6 indices represents one quad
    1, 0, 2, 3, 1, 2, // Front face
    5, 1, 3, 7, 5, 3, // Right face
    7, 6, 4, 5, 7, 4, // Back face
    0, 4, 6, 2, 0, 6, // Left face
    5, 4, 0, 1, 5, 0, // Top face
    3, 2, 6, 7, 3, 6  // Bottom face
};

static const std::array<glm::vec4, 6> NORMALS = {
    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
};

static const std::array<glm::vec4, 8> VERTEX_POSITIONS = {
    glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
    glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f),
    glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),
    glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
    glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),
    glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
    glm::vec4(0.5f, -0.5f, -0.5f, 1.0f)};

Cube::Cube(glm::vec4 position, TextureInfo textureReference, std::array<glm::vec2, 36> textureCoords)
    : m_Position(position),
      m_TextureReference(textureReference),
      m_TextureCoords(textureCoords)
{
}

Cube::~Cube()
{
}

std::vector<CubeVertex> Cube::GetVisibleVertices(std::array<bool, 6> occludedFaces)
{
  std::vector<CubeVertex> visibleVertices;

  for (int index = 0; index < 36; index++)
    if (!occludedFaces[floorf(index / 6)])
    {
      CubeVertex vertex = {};

      glm::vec4 position = VERTEX_POSITIONS[ELEMENTS[index]];

      vertex.position = glm::vec4(m_Position.x + position.x, m_Position.y + position.y, m_Position.z + position.z, 0.0f),
      vertex.textureCoords = m_TextureCoords[index];
      vertex.normal = NORMALS[index / 6];

      visibleVertices.push_back(vertex);
    }

  return visibleVertices;
}

std::vector<CubeVertex> Cube::GetVisibleVertices(int blockIndex, glm::vec3 position, std::array<glm::vec2, 36> textureCoords, std::array<bool, 6> occludedFaces)
{
  std::vector<CubeVertex> visibleVertices;

  for (int index = 0; index < 36; index++)
    if (blockIndex != WATER || (blockIndex == WATER && index / 6 == 4))
    {
      if (!occludedFaces[floorf(index / 6)])
      {
        CubeVertex vertex = {};

        glm::vec4 vertexPosition = VERTEX_POSITIONS[ELEMENTS[index]];

        vertex.position = glm::vec4(position.x + vertexPosition.x, position.y + vertexPosition.y, position.z + vertexPosition.z, 0.0f),
        vertex.textureCoords = textureCoords[index];
        vertex.normal = NORMALS[index / 6];

        visibleVertices.push_back(vertex);
      }
    }

  return visibleVertices;
}
