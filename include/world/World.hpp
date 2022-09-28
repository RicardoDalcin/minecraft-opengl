#ifndef _WORLD_H
#define _WORLD_H

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "entity/Camera.hpp"

#include "world/Chunk.hpp"
#include "world/WorldConstants.hpp"

class World
{
private:
  Shader *m_Shader;
  Texture *m_TextureAtlas;

  std::array<std::array<Chunk *, WorldConstants::CHUNKS_PER_AXIS>, WorldConstants::CHUNKS_PER_AXIS> m_Chunks;

  std::vector<glm::vec2> m_ChunksToUpdate;

  std::array<Chunk *, 4> GetNeighbors(glm::vec2 position)
  {
    int x = position.x;
    int z = position.y;

    std::array<Chunk *, 4> neighbors;

    neighbors[0] = x - 1 >= 0 ? m_Chunks[x - 1][z] : NULL;
    neighbors[1] = z + 1 < WorldConstants::CHUNKS_PER_AXIS ? m_Chunks[x][z + 1] : NULL;
    neighbors[2] = x + 1 < WorldConstants::CHUNKS_PER_AXIS ? m_Chunks[x + 1][z] : NULL;
    neighbors[3] = z - 1 >= 0 ? m_Chunks[x][z - 1] : NULL;

    return neighbors;
  }

public:
  World(Shader *shader);
  ~World();

  static bool RayCastCallback(World *data, glm::vec4 position);

  void UpdateChunkMesh(glm::vec2 position);

  void UpdateMeshes();
  void Draw(Camera *camera, glm::mat4 view, glm::mat4 projection);

  void SetBlock(glm::vec3 position, int block);
  int GetBlock(glm::vec3 position);

  Chunk *GetChunk(int x, int z);

  Texture *GetTextureAtlas() { return m_TextureAtlas; }
};

#endif