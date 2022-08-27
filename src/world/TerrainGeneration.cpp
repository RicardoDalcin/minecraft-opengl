#include "world/TerrainGeneration.hpp"

#include "world/BlockDatabase.hpp"

Noise TerrainGeneration::baseNoise = Noise(8500, 6, 105.0f, 205.0f, 0.58f, 18);
Noise TerrainGeneration::accentNoise = Noise(8500, 4, 20.0f, 200.0f, 0.45f, 0);

int TerrainGeneration::GetHeight(glm::vec2 blockPosition, glm::vec2 chunkPosition)
{
  auto noise = baseNoise.GetNoise(blockPosition, chunkPosition);
  auto noise2 = accentNoise.GetNoise(blockPosition, chunkPosition);

  auto island = GetIslandHeight(blockPosition, chunkPosition) * 1.25;

  float result = noise * noise2;

  int amplitude = baseNoise.GetAmplitude();
  int offset = baseNoise.GetOffset();

  int height = static_cast<int>((result * amplitude + offset) * island) - 5;

  return glm::max(height, WorldConstants::MIN_HEIGHT);
}

int TerrainGeneration::GetBlockAtHeight(int y, int height)
{
  int ores[6] = {IRON_ORE, COAL_ORE, DIAMOND_ORE, REDSTONE_ORE, GOLD_ORE, LAPIS_ORE};

  int heightToStone = rand() % 3 + 2;

  if (y <= height)
  {
    if (y == height)
    {
      return height > WorldConstants::WATER_LEVEL + 3 ? GRASS : SAND;
    }
    else
    {
      if (y == 0)
      {
        return BEDROCK;
      }
      else if (y <= height - heightToStone)
      {
        int willHaveOre = rand() % 20;

        if (willHaveOre == 5)
        {
          int oreIndex = rand() % 5;
          return ores[oreIndex];
        }
        else
        {
          return STONE;
        }
      }
      else
      {
        return DIRT;
      }
    }
  }
  else
  {
    if (y > height && y <= WorldConstants::WATER_LEVEL)
    {
      return WATER;
    }
    else
    {
      return AIR;
    }
  }

  // for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
  // {
  //   for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
  //   {
  //     int height = heightMap[z * WorldConstants::CHUNK_SIZE + x];
  //     if (height > WorldConstants::WATER_LEVEL + 3)
  //     {
  //       int willHaveTree = rand() % 100;
  //       if (willHaveTree == 5)
  //       {
  //         int treeHeight = rand() % 3 + 4;
  //         for (int y = 0; y < treeHeight; y++)
  //         {
  //           if (height + y + 1 < WorldConstants::CHUNK_HEIGHT)
  //           {
  //             m_Cubes[x][height + y + 1][z] = OAK_LOG;
  //           }
  //         }

  //         for (int i = -2; i < 3; i++)
  //         {
  //           for (int j = -2; j < 3; j++)
  //           {
  //             if (i == 0 && j == 0)
  //             {
  //               continue;
  //             }
  //             if (x + i >= 0 && x + i < WorldConstants::CHUNK_SIZE && z + j >= 0 && z + j < WorldConstants::CHUNK_SIZE && height + treeHeight < WorldConstants::CHUNK_HEIGHT)
  //             {
  //               m_Cubes[x + i][height + treeHeight][z + j] = OAK_LEAVES;
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
}