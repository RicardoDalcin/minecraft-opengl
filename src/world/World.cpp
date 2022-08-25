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
      m_Chunks[x][z] = new Chunk(shader);
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
  for (int x = 0; x < CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < CHUNKS_PER_AXIS; z++)
    {
      glm::mat4 model = Matrix_Translate(x * Chunk::CHUNK_SIZE, 0.0f, z * Chunk::CHUNK_SIZE);
      m_Chunks[x][z]->Draw(model, view, projection);
    }
  }
}