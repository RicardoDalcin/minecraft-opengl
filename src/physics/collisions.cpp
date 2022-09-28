#include "physics/collisions.hpp"

namespace Collisions
{
  bool aabbWorldCollision(glm::vec3 playerPosition, glm::vec3 playerSize, World *world)
  {
    int playerMinX = (int)floorf(playerPosition.x - playerSize.x / 2.0f);
    int playerMaxX = (int)floorf(playerPosition.x + playerSize.x / 2.0f);

    int playerMinZ = (int)floorf(playerPosition.z - playerSize.z / 2.0f);
    int playerMaxZ = (int)floorf(playerPosition.z + playerSize.z / 2.0f);

    glm::vec3 bboxCorners[4] = {
        glm::vec3(playerMinX, playerPosition.y, playerMinZ),
        glm::vec3(playerMinX, playerPosition.y, playerMaxZ),
        glm::vec3(playerMaxX, playerPosition.y, playerMinZ),
        glm::vec3(playerMaxX, playerPosition.y, playerMaxZ)};

    for (int i = 0; i < 4; i++)
    {
      glm::vec3 corner = bboxCorners[i];

      int playerChunkX = (int)floorf(corner.x / WorldConstants::CHUNK_SIZE);
      int playerChunkZ = (int)floorf(corner.z / WorldConstants::CHUNK_SIZE);

      if (playerChunkX >= 0 && playerChunkX < WorldConstants::CHUNKS_PER_AXIS && playerChunkZ >= 0 && playerChunkZ < WorldConstants::CHUNKS_PER_AXIS)
      {
        Chunk *chunk = world->GetChunk(playerChunkX, playerChunkZ);

        int playerBlockX = ((int)floorf(corner.x)) % WorldConstants::CHUNK_SIZE;
        int playerBlockZ = ((int)floorf(corner.z)) % WorldConstants::CHUNK_SIZE;

        int maxPlayerBlockY = (int)floorf(corner.y);
        int minPlayerBlockY = (int)ceilf(corner.y - playerSize.y);

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
    }

    return false;
  }
}