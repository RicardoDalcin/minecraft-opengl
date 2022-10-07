#ifndef _TERRAINGENERATION_H
#define _TERRAINGENERATION_H

#include "world/Noise.hpp"
#include "world/WorldConstants.hpp"

// Classe para geração de terreno
class TerrainGeneration
{
private:
  static Noise baseNoise;
  static Noise accentNoise;

  // Gera um valor de ruído para deixar o terreno em forma de ilha
  static float GetIslandHeight(glm::vec2 blockPosition, glm::vec2 chunkPosition, float islandFactor = 6.0f)
  {
    int blockX = blockPosition.x;
    int blockZ = blockPosition.y;

    int chunkX = chunkPosition.x;
    int chunkZ = chunkPosition.y;

    float absoluteX = static_cast<float>(blockX + chunkX * WorldConstants::CHUNK_SIZE);
    float absoluteZ = static_cast<float>(blockZ + chunkZ * WorldConstants::CHUNK_SIZE);

    glm::vec2 islandCoordinate = (glm::vec2(absoluteX, absoluteZ) - WorldConstants::WORLD_SIZE / 2.0f) / WorldConstants::WORLD_SIZE * 2.0f;

    auto bump = [islandFactor](float t)
    { return glm::max(0.0f, 1.0f - std::pow(t, islandFactor)); };

    float bumpedValue = bump(islandCoordinate.x) * bump(islandCoordinate.y);

    return bumpedValue * 0.9f;
  }

public:
  static int GetHeight(glm::vec2 blockPosition, glm::vec2 chunkPosition);
  static int GetBlockAtHeight(int y, int height);
};

#endif