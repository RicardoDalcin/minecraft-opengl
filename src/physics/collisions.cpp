#include "physics/collisions.hpp"

// Namespace para funções de teste de colisão
namespace Collisions
{
  // Retorna o sinal de um número
  int Sign(float x)
  {
    return x > 0 ? 1 : x < 0 ? -1
                             : 0;
  }

  // Função de intbound para o raycasting
  glm::vec3 Intbound(glm::vec3 point, glm::vec3 direction)
  {
    glm::vec3 bound;

    for (size_t i = 0; i < 3; i++)
    {
      bound[i] = (direction[i] > 0 ? (ceilf(point[i]) - point[i]) : (point[i] - floorf(point[i]))) / fabsf(direction[i]);
    }

    return bound;
  }

  // Testa colisão de uma bounding box com o mundo
  bool BoundingBoxWorldCollision(glm::vec3 entityPosition, glm::vec3 entitySize, World *world)
  {
    int bboxMinX = (int)floorf(entityPosition.x - entitySize.x / 2.0f);
    int bboxMaxX = (int)floorf(entityPosition.x + entitySize.x / 2.0f);

    int bboxMinZ = (int)floorf(entityPosition.z - entitySize.z / 2.0f);
    int bboxMaxZ = (int)floorf(entityPosition.z + entitySize.z / 2.0f);

    glm::vec3 bboxCorners[4] = {
        glm::vec3(bboxMinX, entityPosition.y, bboxMinZ),
        glm::vec3(bboxMinX, entityPosition.y, bboxMaxZ),
        glm::vec3(bboxMaxX, entityPosition.y, bboxMinZ),
        glm::vec3(bboxMaxX, entityPosition.y, bboxMaxZ)};

    // Para cada canto da bounding box
    for (int i = 0; i < 4; i++)
    {
      glm::vec3 corner = bboxCorners[i];

      // Calcula qual chunk e bloco a posição do canto está
      int bboxChunkX = (int)floorf(corner.x / WorldConstants::CHUNK_SIZE);
      int bboxChunkZ = (int)floorf(corner.z / WorldConstants::CHUNK_SIZE);

      // Se está em uma posição válida
      if (bboxChunkX >= 0 && bboxChunkX < WorldConstants::CHUNKS_PER_AXIS && bboxChunkZ >= 0 && bboxChunkZ < WorldConstants::CHUNKS_PER_AXIS)
      {
        Chunk *chunk = world->GetChunk(bboxChunkX, bboxChunkZ);

        int bboxBlockX = ((int)floorf(corner.x)) % WorldConstants::CHUNK_SIZE;
        int bboxBlockZ = ((int)floorf(corner.z)) % WorldConstants::CHUNK_SIZE;

        int maxBboxBlockY = (int)floorf(corner.y);
        int minBboxBlockY = (int)ceilf(corner.y - entitySize.y);

        // Testa se há um bloco sólido na posição do canto
        if (bboxBlockX >= 0 && bboxBlockX < WorldConstants::CHUNK_SIZE && bboxBlockZ >= 0 && bboxBlockZ < WorldConstants::CHUNK_SIZE)
        {
          for (int y = maxBboxBlockY; y >= minBboxBlockY; y--)
          {
            int block = chunk->GetCube(glm::vec3(bboxBlockX, y, bboxBlockZ));

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(block);

            if (blockInfo.isSolid)
            {
              return true;
            }
          }
        }
      }
    }

    // Se não detectou nehuma colisão, retorna falso
    return false;
  }

  // Testa colisão entre um ponto e o mundo
  bool PointWorldCollision(glm::vec3 point, World *world)
  {
    int pointChunkX = (int)floorf(point.x / WorldConstants::CHUNK_SIZE);
    int pointChunkZ = (int)floorf(point.z / WorldConstants::CHUNK_SIZE);

    if (pointChunkX >= 0 && pointChunkX < WorldConstants::CHUNKS_PER_AXIS && pointChunkZ >= 0 && pointChunkZ < WorldConstants::CHUNKS_PER_AXIS)
    {
      Chunk *chunk = world->GetChunk(pointChunkX, pointChunkZ);

      int pointBlockX = ((int)floorf(point.x)) % WorldConstants::CHUNK_SIZE;
      int pointBlockZ = ((int)floorf(point.z)) % WorldConstants::CHUNK_SIZE;

      int pointBlockY = (int)floorf(point.y);

      // Se o ponto está em uma posição válida, testa se há um bloco sólido na posição
      if (pointBlockX >= 0 && pointBlockX < WorldConstants::CHUNK_SIZE && pointBlockZ >= 0 && pointBlockZ < WorldConstants::CHUNK_SIZE)
      {
        int block = chunk->GetCube(glm::vec3(pointBlockX, pointBlockY, pointBlockZ));

        BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(block);

        if (blockInfo.isSolid)
        {
          return true;
        }
      }
    }

    // Se não detectou nehuma colisão, retorna falso
    return false;
  }

  // Testa colisão entre um raio e o mundo
  bool RayCast(float length, glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec4), glm::vec3 *out, glm::vec3 *directionOut)
  {
    glm::vec3 step = glm::vec3(Sign(direction.x), Sign(direction.y), Sign(direction.z));

    glm::vec3 rayMax = Intbound(origin, direction);

    glm::vec3 rayDelta = glm::vec3(step.x / direction.x, step.y / direction.y, step.z / direction.z);

    float radius = length / Matrices::Norm(direction);

    while (true)
    {
      // Testa pelo callback do mundo se existe um bloco na posição atual
      if (callback(data, origin))
      {
        *out = origin;
        return true;
      }

      // Se não, "marcha" o raio na direção do passo e verifica se já passou da distância máxima
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

    // Se não detectou nehuma colisão, retorna falso
    return false;
  }
}