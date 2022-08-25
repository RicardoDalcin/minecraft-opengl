#include "world/World.hpp"

#include "core/matrices.hpp"

World::World(Shader *shader)
    : m_Shader(shader),
      m_TextureAtlas(new Texture("extras/textures/atlas.png", true))
{
  for (int x = 0; x < CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < CHUNKS_PER_AXIS; z++)
    {
      m_Chunks[x][z] = new Chunk();
    }
  }
}

World::~World()
{
  for (int x = 0; x < CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < CHUNKS_PER_AXIS; z++)
    {
      delete m_Chunks[x][z];
    }
  }
}

void World::Draw(glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  m_TextureAtlas->Bind(0);
  m_Shader->SetUniform1i("uTexture", 0);

  for (int x = 0; x < CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < CHUNKS_PER_AXIS; z++)
    {
      glm::mat4 model = Matrix_Translate(x * Chunk::CHUNK_SIZE, 0.0f, z * Chunk::CHUNK_SIZE);

      m_Shader->SetUniformMat4f("uTransform", model);

      m_Chunks[x][z]->Draw();
    }
  }
}