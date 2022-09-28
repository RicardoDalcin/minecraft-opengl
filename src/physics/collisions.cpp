#include "physics/collisions.hpp"

namespace Collisions
{
  bool aabbWorldCollision(glm::vec3 playerPosition, glm::vec3 playerSize, World *world)
  {
    int playerChunkX = (int)floorf(playerPosition.x / WorldConstants::CHUNK_SIZE);
    int playerChunkZ = (int)floorf(playerPosition.z / WorldConstants::CHUNK_SIZE);

    if (playerChunkX >= 0 && playerChunkX < WorldConstants::CHUNKS_PER_AXIS && playerChunkZ >= 0 && playerChunkZ < WorldConstants::CHUNKS_PER_AXIS)
    {
      Chunk *chunk = world->GetChunk(playerChunkX, playerChunkZ);

      int playerBlockX = ((int)floorf(playerPosition.x)) % WorldConstants::CHUNK_SIZE;
      int playerBlockZ = ((int)floorf(playerPosition.z)) % WorldConstants::CHUNK_SIZE;

      int maxPlayerBlockY = (int)floorf(playerPosition.y);
      int minPlayerBlockY = (int)ceilf(playerPosition.y - playerSize.y);

      if (playerBlockX >= 0 && playerBlockX < WorldConstants::CHUNK_SIZE && playerBlockZ >= 0 && playerBlockZ < WorldConstants::CHUNK_SIZE)
      {
        for (int y = maxPlayerBlockY; y >= minPlayerBlockY; y--)
        {
          int block = chunk->GetCube(glm::vec3(playerBlockX, y, playerBlockZ));

          BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(block);

          if (blockInfo.isSolid)
          {
            return true;
          }
        }
      }
    }

    return false;
  }
}