#include <algorithm>

#include "world/World.hpp"

#include "core/Matrices.hpp"

// Inicializa o mundo
World::World(Shader *shader)
    : m_Shader(shader),
      m_TextureAtlas(new Texture("extras/textures/atlas.png", true))
{
  // Adiciona os chunks na lista de atualização
  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      m_Chunks[x][z] = new Chunk(x, z);
      m_ChunksToUpdate.push_back(glm::vec2(x, z));
    }
  }

  // Constrói as meshes dos chunks
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

// Atualiza o mesh de um chunk
void World::UpdateChunkMesh(glm::vec2 position)
{
  int x = position.x;
  int z = position.y;

  std::array<Chunk *, 4> neighbors = GetNeighbors(position);

  m_Chunks[x][z]->BuildMesh(neighbors);
}

// Atualiza a mesh dos chunks que estão na lista de atualização
void World::UpdateMeshes()
{
  for (auto &position : m_ChunksToUpdate)
    UpdateChunkMesh(position);

  m_ChunksToUpdate.clear();
}

// Desenha o mundo
void World::Draw(Camera *camera, glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  m_TextureAtlas->Bind(0);
  m_Shader->SetUniform1i("uTexture", 0);

  glm::vec4 cameraPosition = camera->GetPosition();

  std::vector<std::pair<float, Chunk *>> chunks;

  for (int x = 0; x < WorldConstants::CHUNKS_PER_AXIS; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNKS_PER_AXIS; z++)
    {
      Chunk *chunk = m_Chunks[x][z];

      float distance = glm::distance(glm::vec2(cameraPosition.x, cameraPosition.z), glm::vec2(x * WorldConstants::CHUNK_SIZE + WorldConstants::CHUNK_SIZE / 2, z * WorldConstants::CHUNK_SIZE + WorldConstants::CHUNK_SIZE / 2));

      chunks.push_back(std::make_pair(distance, chunk));
    }
  }

  // Ordena os chunks de acordo com a distância da câmera (mais longe )
  std::sort(chunks.begin(), chunks.end(), [](const std::pair<float, Chunk *> &a, const std::pair<float, Chunk *> &b)
            { return a.first > b.first; });

  // Com os chunks ordenados, para cada chunk chama a função de renderização
  for (auto &chunk : chunks)
  {
    int chunkX = chunk.second->GetChunkX();
    int chunkZ = chunk.second->GetChunkZ();

    // Translação para a posição do chunk
    glm::mat4 model = Matrices::MatrixTranslate(chunkX * WorldConstants::CHUNK_SIZE, 0.0f, chunkZ * WorldConstants::CHUNK_SIZE);

    m_Shader->SetUniformMat4f("uTransform", model);

    chunk.second->Draw(m_Shader);
  }
}

// Callback de raycast do mundo
bool World::RayCastCallback(World *data, glm::vec4 position)
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

  // Se a posição de raycast é válida, busca o chunk
  Chunk *chunk = data->m_Chunks[chunkX][chunkZ];

  int blockY = (int)position.y;

  if (blockY < 0 || blockY >= WorldConstants::CHUNK_HEIGHT)
  {
    return false;
  }

  // Se o bloco na posição de raycast não é ar ou água, retorna true
  int block = chunk->GetCube(glm::vec3(blockX, blockY, blockZ));

  return block != AIR && block != WATER;
}

// Atualiza um bloco no mundo
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

  // Adiciona o chunk modificado na lista de atualização
  m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ));

  // Adiciona os chunks vizinhos na lista de atualização, se necessário
  if (blockX == 0 && chunkX - 1 >= 0)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX - 1, chunkZ));

  if (blockX == WorldConstants::CHUNK_SIZE - 1 && chunkX + 1 < WorldConstants::CHUNKS_PER_AXIS)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX + 1, chunkZ));

  if (blockZ == 0 && chunkZ - 1 >= 0)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ - 1));

  if (blockZ == WorldConstants::CHUNK_SIZE - 1 && chunkZ + 1 < WorldConstants::CHUNKS_PER_AXIS)
    m_ChunksToUpdate.push_back(glm::vec2(chunkX, chunkZ + 1));

  // Atualiza a mesh dos chunks
  UpdateMeshes();
}

// Retorna o bloco na posição especificada
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

Chunk *World::GetChunk(int chunkX, int chunkZ)
{
  if (chunkX < 0 || chunkX >= WorldConstants::CHUNKS_PER_AXIS || chunkZ < 0 || chunkZ >= WorldConstants::CHUNKS_PER_AXIS)
  {
    return nullptr;
  }

  return m_Chunks[chunkX][chunkZ];
}