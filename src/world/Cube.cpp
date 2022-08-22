#include <glad/glad.h>

#include "world/Cube.hpp"

// static const std::array<uint32_t, 36> ELEMENTS = {
//     // Each set of 6 indices represents one quad
//     1, 0, 2, 3, 1, 2, // Front face
//     5, 1, 3, 7, 5, 3, // Right face
//     7, 6, 4, 5, 7, 4, // Back face
//     0, 4, 6, 2, 0, 6, // Left face
//     5, 4, 0, 1, 5, 0, // Top face
//     3, 2, 6, 7, 3, 6  // Bottom face
// };

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

Cube::Cube(glm::vec4 position, const Texture *texture, int textureIndex, Shader *shader)
    : m_Elements(ELEMENTS),
      m_Normals(NORMALS),
      m_TextureIndex(textureIndex)
{
  m_Position = position;
  m_Texture = texture;
  m_Shader = shader;

  std::array<glm::vec4, 8> vertexPositions = {
      glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),
      glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
      glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f),
      glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),
      glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f),
      glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),
      glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f),
      glm::vec4(0.5f, -0.5f, -0.5f, 1.0f)};

  int horizontalTextureCount = 32;
  int verticalTextureCount = 11;

  int textureIndexX = textureIndex % horizontalTextureCount;
  int textureIndexY = textureIndex / verticalTextureCount;

  // texture 0
  //  0.0f 1.0f / 32
  //  0.0f 1.0f / 11

  // texture 1
  //  0.0f + 1.0f / 32

  float unitX = 1.0f / horizontalTextureCount;
  float unitY = 1.0f / verticalTextureCount;

  float textureZeroX = unitX * textureIndexX;
  float textureZeroY = unitY * ((verticalTextureCount - 1) - textureIndexY);

  float textureOneX = textureZeroX + unitX;
  float textureOneY = textureZeroY + unitY;

  std::array<glm::vec2, 6> textureCoords;
  textureCoords[0] = glm::vec2(textureOneX, textureOneY);
  textureCoords[1] = glm::vec2(textureZeroX, textureOneY);
  textureCoords[2] = glm::vec2(textureZeroX, textureZeroY);

  textureCoords[4] = glm::vec2(textureOneX, textureOneY);
  textureCoords[3] = glm::vec2(textureOneX, textureZeroY);
  textureCoords[5] = glm::vec2(textureZeroX, textureZeroY);

  // std::array<glm::vec2, 6> textureCoords;
  // textureCoords[0] = glm::vec2(1.0f, 1.0f);
  // textureCoords[1] = glm::vec2(0.0f, 1.0f);
  // textureCoords[2] = glm::vec2(0.0f, 0.0f);

  // textureCoords[4] = glm::vec2(1.0f, 1.0f);
  // textureCoords[3] = glm::vec2(1.0f, 0.0f);
  // textureCoords[5] = glm::vec2(0.0f, 0.0f);

  for (int index = 0; index < m_Elements.size(); index++)
  {
    m_Vertices[index].position = vertexPositions[m_Elements[index]];
    m_Vertices[index].textureCoords = textureCoords[index % 6];
    m_Vertices[index].normal = m_Normals[index / 6];
  }

  m_VAO = new VertexArray();
  m_VAO->Bind();

  m_VBO = new VertexBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(CubeVertex));

  VertexBufferLayout *layout = new VertexBufferLayout();

  layout->Push(LayoutType::LT_FLOAT, 4);
  layout->Push(LayoutType::LT_FLOAT, 2);
  layout->Push(LayoutType::LT_FLOAT, 4);

  m_VAO->AddBuffer(*m_VBO, *layout);
}

Cube::~Cube()
{
  delete m_VBO;
  delete m_VAO;
}

void Cube::Draw(glm::mat4 view, glm::mat4 projection)
{
  glm::mat4 transform = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z);
  m_Shader->SetUniformMat4f("uTransform", transform);

  m_Texture->Bind(0);
  m_Shader->SetUniform1i("uTexture", 0);

  m_VAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
}
