#include "physics/collisions.hpp"

namespace Collisions
{
  int Sign(float x)
  {
    return x > 0 ? 1 : x < 0 ? -1
                             : 0;
  }

  glm::vec3 Intbound(glm::vec3 point, glm::vec3 direction)
  {
    glm::vec3 bound;

    for (size_t i = 0; i < 3; i++)
    {
      bound[i] = (direction[i] > 0 ? (ceilf(point[i]) - point[i]) : (point[i] - floorf(point[i]))) / fabsf(direction[i]);
    }

    return bound;
  }

  bool BoundingBoxWorldCollision(glm::vec3 entityPosition, glm::vec3 entitySize, World *world)
  {
    int playerMinX = (int)floorf(entityPosition.x - entitySize.x / 2.0f);
    int playerMaxX = (int)floorf(entityPosition.x + entitySize.x / 2.0f);

    int playerMinZ = (int)floorf(entityPosition.z - entitySize.z / 2.0f);
    int playerMaxZ = (int)floorf(entityPosition.z + entitySize.z / 2.0f);

    glm::vec3 bboxCorners[4] = {
        glm::vec3(playerMinX, entityPosition.y, playerMinZ),
        glm::vec3(playerMinX, entityPosition.y, playerMaxZ),
        glm::vec3(playerMaxX, entityPosition.y, playerMinZ),
        glm::vec3(playerMaxX, entityPosition.y, playerMaxZ)};

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
        int minPlayerBlockY = (int)ceilf(corner.y - entitySize.y);

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

  bool RayCast(float length, glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec4), glm::vec3 *out, glm::vec3 *directionOut)
  {

    glm::vec3 step = glm::vec3(Sign(direction.x), Sign(direction.y), Sign(direction.z));

    glm::vec3 rayMax = Intbound(origin, direction);

    glm::vec3 rayDelta = glm::vec3(step.x / direction.x, step.y / direction.y, step.z / direction.z);

    float radius = length / Matrices::Norm(direction);

    while (true)
    {
      if (callback(data, origin))
      {
        *out = origin;
        return true;
      }

      if (rayMax.x < rayMax.y)
      {
        if (rayMax.x < rayMax.z)
        {
          if (rayMax.x > radius)
          {
            break;
          }

          origin.x += step.x;
          rayMax.x += rayDelta.x;
          *directionOut = glm::vec3(-step.x, 0.0f, 0.0f);
        }
        else
        {
          if (rayMax.z > radius)
          {
            break;
          }

          origin.z += step.z;
          rayMax.z += rayDelta.z;
          *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
        }
      }
      else
      {
        if (rayMax.y < rayMax.z)
        {
          if (rayMax.y > radius)
          {
            break;
          }

          origin.y += step.y;
          rayMax.y += rayDelta.y;
          *directionOut = glm::vec3(0.0f, -step.y, 0.0f);
        }
        else
        {
          if (rayMax.z > radius)
          {
            break;
          }

          origin.z += step.z;
          rayMax.z += rayDelta.z;
          *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
        }
      }
    }

    return false;
  }
}