#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glad/glad.h>

#include "world/Chunk.hpp"

#include "world/BlockDatabase.hpp"

struct NoiseOptions
{
  int octaves;
  float amplitude;
  float smoothness;
  float roughness;
  float offset;
};

float getNoiseAt(const glm::vec2 &voxelPosition, const glm::vec2 &chunkPosition, NoiseOptions &options, int seed)
{
  // Get voxel X/Z positions
  float voxelX = voxelPosition.x + chunkPosition.x * WorldConstants::CHUNK_SIZE;
  float voxelZ = voxelPosition.y + chunkPosition.y * WorldConstants::CHUNK_SIZE;

  // Begin iterating through the octaves
  float value = 0;
  float accumulatedAmps = 0;
  for (int i = 0; i < options.octaves; i++)
  {
    float frequency = glm::pow(2.0f, i);
    float amplitude = glm::pow(options.roughness, i);

    float x = voxelX * frequency / options.smoothness;
    float y = voxelZ * frequency / options.smoothness;

    float noise = glm::simplex(glm::vec3{seed + x, seed + y, seed});
    noise = (noise + 1.0f) / 2.0f;
    value += noise * amplitude;
    accumulatedAmps += amplitude;
  }
  return value / accumulatedAmps;
}

float rounded(const glm::vec2 &coord)
{
  auto bump = [](float t)
  { return glm::max(0.0f, 1.0f - std::pow(t, 6.0f)); };
  float b = bump(coord.x) * bump(coord.y);
  return b * 0.9f;
}

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX),
      m_ChunkZ(chunkZ),
      m_VAO(NULL),
      m_VBO(NULL),
      m_TransparentVAO(NULL),
      m_TransparentVBO(NULL)
{
  int cx = chunkX;
  int cz = chunkZ;

  std::array<int, WorldConstants::CHUNK_SIZE * WorldConstants::CHUNK_SIZE> heightMap;

  const float WORLD_SIZE = static_cast<float>(WorldConstants::CHUNKS_PER_AXIS) * WorldConstants::CHUNK_SIZE;

  NoiseOptions firstNoise;
  firstNoise.amplitude = 105;
  firstNoise.octaves = 6;
  firstNoise.smoothness = 205.f;
  firstNoise.roughness = 0.58f;
  firstNoise.offset = 18;

  NoiseOptions secondNoise;
  secondNoise.amplitude = 20;
  secondNoise.octaves = 4;
  secondNoise.smoothness = 200;
  secondNoise.roughness = 0.45f;
  secondNoise.offset = 0;

  for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
  {
    for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
    {
      float bx = static_cast<float>(x + cx * WorldConstants::CHUNK_SIZE);
      float bz = static_cast<float>(z + cz * WorldConstants::CHUNK_SIZE);

      glm::vec2 coord = (glm::vec2(bx, bz) - WORLD_SIZE / 2.0f) / WORLD_SIZE * 2.0f;

      auto noise = getNoiseAt({x, z}, glm::vec2(cx, cz), firstNoise, 8500);
      auto noise2 = getNoiseAt({x, z}, {cx, cz}, secondNoise, 8500);

      auto island = rounded(coord) * 1.25;
      float result = noise * noise2;

      heightMap[x + z * WorldConstants::CHUNK_SIZE] = glm::max(static_cast<int>((result * firstNoise.amplitude + firstNoise.offset) *
                                                                                island) -
                                                                   5,
                                                               2);
    }

    int ores[6] = {IRON_ORE, COAL_ORE, DIAMOND_ORE, REDSTONE_ORE, GOLD_ORE, LAPIS_ORE};

    for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
    {
      for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
      {
        int height = heightMap[z * WorldConstants::CHUNK_SIZE + x];

        int heightToStone = rand() % 3 + 2;

        for (int y = 0; y < WorldConstants::CHUNK_HEIGHT; y++)
        {
          if (y <= height)
          {
            if (y == height)
            {
              m_Cubes[x][y][z] = height > WorldConstants::WATER_LEVEL + 3 ? GRASS : SAND;
            }
            else
            {
              if (y == 0)
              {
                m_Cubes[x][y][z] = BEDROCK;
              }
              else if (y <= height - heightToStone)
              {
                int willHaveOre = rand() % 20;

                if (willHaveOre == 5)
                {
                  int oreIndex = rand() % 5;
                  m_Cubes[x][y][z] = ores[oreIndex];
                }
                else
                {
                  m_Cubes[x][y][z] = STONE;
                }
              }
              else
              {
                m_Cubes[x][y][z] = DIRT;
              }
            }
          }
          else
          {
            if (y > height && y <= WorldConstants::WATER_LEVEL)
            {
              m_Cubes[x][y][z] = WATER;
            }
            else
            {
              m_Cubes[x][y][z] = AIR;
            }
          }
        }
      }
    }
  }
}

Chunk::~Chunk()
{
}

void Chunk::BuildMesh(std::array<Chunk *, 4> neighbors)
{
  if (m_VAO != NULL)
  {
    delete m_VAO;
  }

  m_VAO = new VertexArray();

  if (m_TransparentVAO != NULL)
  {
    delete m_TransparentVAO;
  }

  m_TransparentVAO = new VertexArray();

  std::vector<CubeVertex> vertices;
  std::vector<CubeVertex> transparentVertices;

  for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
  {
    for (int y = 0; y < WorldConstants::CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
      {
        int cube = m_Cubes[x][y][z];

        if (cube != 0)
        {
          bool hasBlockInFront = false;
          bool hasBlockInRight = false;
          bool hasBlockInBack = false;
          bool hasBlockInLeft = false;
          bool hasBlockInTop = false;
          bool hasBlockInBottom = false;

          if (z + 1 < WorldConstants::CHUNK_SIZE)
          {
            int blockInFront = m_Cubes[x][y][z + 1];
            hasBlockInFront = cube != WATER ? blockInFront != AIR && blockInFront != WATER : blockInFront != AIR;
          }
          else if (neighbors[1] != NULL)
          {
            int blockInFront = neighbors[1]->m_Cubes[x][y][0];
            hasBlockInFront = cube != WATER ? blockInFront != AIR && blockInFront != WATER : blockInFront != AIR;
          }

          if (x + 1 < WorldConstants::CHUNK_SIZE)
          {
            int blockInRight = m_Cubes[x + 1][y][z];
            hasBlockInRight = cube != WATER ? blockInRight != AIR && blockInRight != WATER : blockInRight != AIR;
          }
          else if (neighbors[2] != NULL)
          {
            int blockInRight = neighbors[2]->m_Cubes[0][y][z];
            hasBlockInRight = cube != WATER ? blockInRight != AIR && blockInRight != WATER : blockInRight != AIR;
          }

          if (z - 1 >= 0)
          {
            int blockInBack = m_Cubes[x][y][z - 1];
            hasBlockInBack = cube != WATER ? blockInBack != AIR && blockInBack != WATER : blockInBack != AIR;
          }
          else if (neighbors[3] != NULL)
          {
            int blockInBack = neighbors[3]->m_Cubes[x][y][WorldConstants::CHUNK_SIZE - 1];
            hasBlockInBack = cube != WATER ? blockInBack != AIR && blockInBack != WATER : blockInBack != AIR;
          }

          if (x - 1 >= 0)
          {
            int blockInLeft = m_Cubes[x - 1][y][z];
            hasBlockInLeft = cube != WATER ? blockInLeft != AIR && blockInLeft != WATER : blockInLeft != AIR;
          }
          else if (neighbors[0] != NULL)
          {
            int blockInLeft = neighbors[0]->m_Cubes[WorldConstants::CHUNK_SIZE - 1][y][z];
            hasBlockInLeft = cube != WATER ? blockInLeft != AIR && blockInLeft != WATER : blockInLeft != AIR;
          }

          if (y + 1 < WorldConstants::CHUNK_HEIGHT)
          {
            int blockInTop = m_Cubes[x][y + 1][z];
            hasBlockInTop = cube != WATER ? blockInTop != AIR && blockInTop != WATER : blockInTop != AIR;
          }

          if (y - 1 >= 0)
          {
            int blockInBottom = m_Cubes[x][y - 1][z];
            hasBlockInBottom = cube != WATER ? blockInBottom != AIR && blockInBottom != WATER : blockInBottom != AIR;
          }

          std::array<bool, 6> occlusion = {
              hasBlockInFront,
              hasBlockInRight,
              hasBlockInBack,
              hasBlockInLeft,
              hasBlockInTop,
              hasBlockInBottom};

          BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(cube);

          std::vector<CubeVertex> visibleVertices = Cube::GetVisibleVertices(cube, glm::vec3(x, y, z), blockInfo.textureCoordinates, occlusion);

          if (blockInfo.isOpaque)
            vertices.insert(vertices.end(), visibleVertices.begin(), visibleVertices.end());
          else
            transparentVertices.insert(transparentVertices.end(), visibleVertices.begin(), visibleVertices.end());
        }
      }
    }
  }

  if (m_VBO != NULL)
    delete m_VBO;

  if (m_TransparentVBO != NULL)
    delete m_TransparentVBO;

  m_VBO = new VertexBuffer(&vertices[0], vertices.size() * sizeof(CubeVertex));
  m_TransparentVBO = new VertexBuffer(&transparentVertices[0], transparentVertices.size() * sizeof(CubeVertex));

  VertexBufferLayout *layout = new VertexBufferLayout();

  layout->Push(LayoutType::LT_FLOAT, 4);
  layout->Push(LayoutType::LT_FLOAT, 2);
  layout->Push(LayoutType::LT_FLOAT, 4);

  m_VAO->AddBuffer(*m_VBO, *layout);
  m_TransparentVAO->AddBuffer(*m_TransparentVBO, *layout);

  m_MeshVertexCount = vertices.size();
  m_TransparentMeshVertexCount = transparentVertices.size();
}

void Chunk::Draw()
{
  m_VAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_MeshVertexCount);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_TransparentVAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_TransparentMeshVertexCount);

  glDisable(GL_BLEND);
}
