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
      m_ChunksToUpdate.push_back(glm::vec2(x, z));
    }
  }

  UpdateMeshes();
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

void World::UpdateChunkMesh(glm::vec2 position)
{
  int x = position.x;
  int z = position.y;

  std::array<Chunk *, 4> neighbors = GetNeighbors(position);

  m_Chunks[x][z]->BuildMesh(neighbors);
}

void World::UpdateMeshes()
{
  for (auto &position : m_ChunksToUpdate)
    UpdateChunkMesh(position);

  m_ChunksToUpdate.clear();
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

bool World::RayCastCallback(World *data, glm::vec3 position)
{
  int chunkX = (int)position.x / WorldConstants::CHUNK_SIZE;
  int chunkZ = (int)position.z / WorldConstants::CHUNK_SIZE;

  int blockX = (int)position.x % WorldConstants::CHUNK_SIZE;
  int blockZ = (int)position.z % WorldConstants::CHUNK_SIZE;

  if (blockX < 0)
  {
    blockX += WorldConstants::CHUNK_SIZE;
    chunkX--;
  }

  if (blockZ < 0)
  {
    blockZ += WorldConstants::CHUNK_SIZE;
    chunkZ--;
  }

  if (chunkX < 0 || chunkX >= WorldConstants::CHUNKS_PER_AXIS || chunkZ < 0 || chunkZ >= WorldConstants::CHUNKS_PER_AXIS)
  {
    return false;
  }

  Chunk *chunk = data->m_Chunks[chunkX][chunkZ];

  int blockY = (int)position.y;

  if (blockY < 0 || blockY >= WorldConstants::CHUNK_HEIGHT)
  {
    return false;
  }

  int block = chunk->GetCube(glm::vec3(blockX, blockY, blockZ));

  return block != AIR && block != WATER;
}

void World::SetBlock(glm::vec3 position, int block)
{
  int chunkX = (int)position.x / WorldConstants::CHUNK_SIZE;
  int chunkZ = (int)position.z / WorldConstants::CHUNK_SIZE;

  int blockX = (int)position.x % WorldConstants::CHUNK_SIZE;
  int blockZ = (int)position.z % WorldConstants::CHUNK_SIZE;

  bool isChunkXValid = chunkX >= 0 && chunkX < WorldConstants::CHUNKS_PER_AXIS;
  bool isChunkZValid = chunkZ >= 0 && chunkZ < WorldConstants::CHUNKS_PER_AXIS;

  if (!isChunkXValid || !isChunkZValid)
    return;

  Chunk *chunk = m_Chunks[chunkX][chunkZ];

  int blockY = (int)position.y;

  bool isBlockYValid = blockY >= 0 && blockY < WorldConstants::CHUNK_HEIGHT;

  if (!isBlockYValid)
    return;

  chunk->SetCube(glm::vec3(blockX, blockY, blockZ), block);

  m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ));

  if (blockX == 0 && chunkX - 1 >= 0)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX - 1, chunkZ));

  if (blockX == WorldConstants::CHUNK_SIZE - 1 && chunkX + 1 < WorldConstants::CHUNKS_PER_AXIS)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX + 1, chunkZ));

  if (blockZ == 0 && chunkZ - 1 >= 0)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ - 1));

  if (blockZ == WorldConstants::CHUNK_SIZE - 1 && chunkZ + 1 < WorldConstants::CHUNKS_PER_AXIS)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ + 1));

  UpdateMeshes();
}

int World::GetBlock(glm::vec3 position)
{
  int chunkX = (int)position.x / WorldConstants::CHUNK_SIZE;
  int chunkZ = (int)position.z / WorldConstants::CHUNK_SIZE;

  int blockX = (int)position.x % WorldConstants::CHUNK_SIZE;
  int blockZ = (int)position.z % WorldConstants::CHUNK_SIZE;

  if (blockX < 0)
  {
    blockX += WorldConstants::CHUNK_SIZE;
    chunkX--;
  }

  if (blockZ < 0)
  {
    blockZ += WorldConstants::CHUNK_SIZE;
    chunkZ--;
  }

  if (chunkX < 0 || chunkX >= WorldConstants::CHUNKS_PER_AXIS || chunkZ < 0 || chunkZ >= WorldConstants::CHUNKS_PER_AXIS)
  {
    return AIR;
  }

  Chunk *chunk = m_Chunks[chunkX][chunkZ];

  int blockY = (int)position.y;

  if (blockY < 0 || blockY >= WorldConstants::CHUNK_HEIGHT)
  {
    return AIR;
  }

  return chunk->GetCube(glm::vec3(blockX, blockY, blockZ));
}