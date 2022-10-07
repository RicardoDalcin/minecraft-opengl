#include "world/TerrainGeneration.hpp"

#include "world/BlockDatabase.hpp"

const int SEED = 8500;

// Inicializa as noises de base e de detalhes
Noise TerrainGeneration::baseNoise = Noise(SEED, 6, 105.0f, 205.0f, 0.58f, 18);
Noise TerrainGeneration::accentNoise = Noise(SEED, 4, 20.0f, 200.0f, 0.45f, 0);

// Calcula a altura do terreno na posição
int TerrainGeneration::GetHeight(glm::vec2 blockPosition, glm::vec2 chunkPosition)
{
  auto noise = baseNoise.GetNoise(blockPosition, chunkPosition);
  auto noise2 = accentNoise.GetNoise(blockPosition, chunkPosition);

  auto island = GetIslandHeight(blockPosition, chunkPosition, 2.0f) * 1.25;

  // Combina as duas noises para gerar um terreno mais interessante
  float result = noise * noise2;

  int amplitude = baseNoise.GetAmplitude();
  int offset = baseNoise.GetOffset();

  // Calcula a altura com base na amplitude, offset e na altura de ilha
  int height = static_cast<int>((result * amplitude + offset) * island) - 5;

  // Retorna o maior valor entre a altura e a altura mínima
  return glm::max(height, WorldConstants::MIN_HEIGHT);
}

// Computa qual bloco deve ser gerado na altura
int TerrainGeneration::GetBlockAtHeight(int y, int height)
{
  int ores[6] = {IRON_ORE, COAL_ORE, DIAMOND_ORE, REDSTONE_ORE, GOLD_ORE, LAPIS_ORE};

  // Gera um valor aleatório para a altura de terra até ter pedra
  int heightToStone = rand() % 3 + 2;

  // Se y é menor ou igual à altura
  if (y <= height)
  {
    // Se y é igual à altura
    if (y == height)
    {
      // Retorna areia se for até 3 blocos acima do nível do mar, senão retorna grama
      return height > WorldConstants::WATER_LEVEL + 3 ? GRASS : SAND;
    }
    else
    {
      // Se está na camada zero, retorna bedrock
      if (y == 0)
      {
        return BEDROCK;
      }
      // Se está embaixo da camada de terra
      else if (y <= height - heightToStone)
      {
        int willHaveOre = rand() % 20;

        // Testa aleatoriamente se vai ter um bloco de minério
        if (willHaveOre == 5)
        {
          int oreIndex = rand() % 5;
          return ores[oreIndex];
        }
        else
        {
          // Senão retorna pedra
          return STONE;
        }
      }
      else
      {
        // Senão retorna terra
        return DIRT;
      }
    }
  }
  else
  {
    // Se o y é maior que a altura, mas menor ou igual ao nível do mar, retorna água
    if (y > height && y <= WorldConstants::WATER_LEVEL)
    {
      return WATER;
    }
    else
    {
      // Senão retorna ar
      return AIR;
    }
  }
}