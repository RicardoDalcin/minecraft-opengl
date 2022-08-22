#include <glad/glad.h>

#include "world/Chunk.hpp"

Chunk::Chunk(Shader *shader)
    : m_Shader(shader),
      m_Textures({new Texture("extras/textures/chiseled_stone_bricks.png"), new Texture("extras/textures/dirt.png"), new Texture("extras/textures/atlas.png")}),
      m_NormalTexture(new Texture("extras/textures/normal.jpg"))
{
  m_Shader->Bind();

  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        int randomTextureIndex = rand() % 30;
        m_Cubes[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] = new Cube(glm::vec4(x, y, z, 1.0f), m_Textures[2], randomTextureIndex, m_Shader);
      }
    }
  }
}

Chunk::~Chunk()
{
  for (const auto &cube : m_Cubes)
  {
    delete cube;
  }

  for (const auto &texture : m_Textures)
  {
    delete texture;
  }
}

void Chunk::Draw(glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  m_NormalTexture->Bind(1);
  m_Shader->SetUniform1i("uNormalTexture", 1);

  for (const auto &cube : m_Cubes)
  {
    cube->Draw(view, projection);
  }
}
