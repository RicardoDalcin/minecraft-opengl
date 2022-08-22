#include <glad/glad.h>

#include "world/Chunk.hpp"

#include "world/BlockDatabase.hpp"

Chunk::Chunk(Shader *shader)
    : m_Shader(shader),
      m_TextureAtlas(new Texture("extras/textures/atlas.png", true))
{
  m_Shader->Bind();

  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        int randomBlockIndex = rand() % 50;

        BlockInformation randomBlock = BlockDatabase::GetBlockInformationIndex(randomBlockIndex);
        // BlockInformation randomBlock = BlockDatabase::GetBlockInformation(y == CHUNK_HEIGHT - 1 ? "grass" : "dirt");

        m_Cubes[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT] =
            new Cube(
                glm::vec4(x, y, z, 1.0f),
                m_TextureAtlas,
                randomBlock.textureReference,
                m_Shader);
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

  delete m_TextureAtlas;
}

void Chunk::Draw(glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  for (const auto &cube : m_Cubes)
  {
    cube->Draw(view, projection);
  }
}
