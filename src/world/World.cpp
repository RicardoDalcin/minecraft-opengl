#include <algorithm>

#include "world/World.hpp"

#include "core/matrices.hpp"

World::World(Shader *shader)
    : m_Shader(shader),
      m_TextureAtlas(new Texture("extras/textures/atlas.png", true))
{
  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      m_Chunks[x][z] = new Chunk(x, z);
    }
  }
}

World::~World()
{
  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      delete m_Chunks[x][z];
    }
  }
}

void World::UpdateMeshes()
{
  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      std::array<Chunk *, 4> neighbors;

      neighbors[0] = x - 1 >= 0 ? m_Chunks[x - 1][z] : NULL;
      neighbors[1] = z + 1 < WorldConstants::CHUNKS_PER_AXIS ? m_Chunks[x][z + 1] : NULL;
      neighbors[2] = x + 1 < WorldConstants::CHUNKS_PER_AXIS ? m_Chunks[x + 1][z] : NULL;
      neighbors[3] = z - 1 >= 0 ? m_Chunks[x][z - 1] : NULL;

      m_Chunks[x][z]->BuildMesh(neighbors);
    }
  }
}

void World::Draw(Camera *camera, glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  m_TextureAtlas->Bind(0);
  m_Shader->SetUniform1i("uTexture", 0);

  glm::vec4 cameraPosition = camera->getPosition();

  std::vector<std::pair<float, Chunk *>> chunks;

  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      Chunk *chunk = m_Chunks[x][z];

      float distance = glm::distance(glm::vec2(cameraPosition.x, cameraPosition.z), glm::vec2(x * WorldConstants::CHUNK_SIZE, z * WorldConstants::CHUNK_SIZE));

      chunks.push_back(std::make_pair(distance, chunk));
    }
  }

  std::sort(chunks.begin(), chunks.end(), [](const std::pair<float, Chunk *> &a, const std::pair<float, Chunk *> &b)
            { return a.first > b.first; });

  for (auto &chunk : chunks)
  {
    int chunkX = chunk.second->GetChunkX();
    int chunkZ = chunk.second->GetChunkZ();

    glm::mat4 model = Matrix_Translate(chunkX * WorldConstants::CHUNK_SIZE, 0.0f, chunkZ * WorldConstants::CHUNK_SIZE);

    m_Shader->SetUniformMat4f("uTransform", model);

    chunk.second->Draw();
  }
}