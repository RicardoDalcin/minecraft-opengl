#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include "core.h"

#include "world/Chunk.hpp"

#include "world/Noise.hpp"
#include "world/TerrainGeneration.hpp"
#include "world/BlockDatabase.hpp"

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX),
      m_ChunkZ(chunkZ),
      m_VAO(NULL),
      m_VBO(NULL),
      m_TransparentVAO(NULL),
      m_TransparentVBO(NULL)
{
  std::array<int, WorldConstants::CHUNK_SIZE * WorldConstants::CHUNK_SIZE> heightMap;

  for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
  {
    for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
    {
      heightMap[x + z * WorldConstants::CHUNK_SIZE] = TerrainGeneration::GetHeight(glm::vec2(x, z), glm::vec2(chunkX, chunkZ));
    }
  }

  for (int x = 0; x < WorldConstants::CHUNK_SIZE; x++)
  {
    for (int z = 0; z < WorldConstants::CHUNK_SIZE; z++)
    {
      for (int y = 0; y < WorldConstants::CHUNK_HEIGHT; y++)
      {
        int block = TerrainGeneration::GetBlockAtHeight(y, heightMap[x + z * WorldConstants::CHUNK_SIZE]);
        m_Cubes[x][y][z] = block;
      }
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

Chunk::~Chunk()
{
  if (m_VAO != NULL)
    delete m_VAO;

  if (m_VBO != NULL)
    delete m_VBO;

  if (m_TransparentVAO != NULL)
    delete m_TransparentVAO;

  if (m_TransparentVBO != NULL)
    delete m_TransparentVBO;
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
          BlockInformation cubeInfo = BlockDatabase::GetBlockInformationIndex(cube);

          bool hasBlockInFront = false;
          bool hasBlockInRight = false;
          bool hasBlockInBack = false;
          bool hasBlockInLeft = false;
          bool hasBlockInTop = false;
          bool hasBlockInBottom = false;

          if (z + 1 < WorldConstants::CHUNK_SIZE)
          {
            int blockInFront = m_Cubes[x][y][z + 1];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInFront);

            hasBlockInFront = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInFront == cube;
          }
          else if (neighbors[1] != NULL)
          {
            int blockInFront = neighbors[1]->m_Cubes[x][y][0];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInFront);

            hasBlockInFront = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInFront == cube;
          }

          if (x + 1 < WorldConstants::CHUNK_SIZE)
          {
            int blockInRight = m_Cubes[x + 1][y][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInRight);

            hasBlockInRight = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInRight == cube;
          }
          else if (neighbors[2] != NULL)
          {
            int blockInRight = neighbors[2]->m_Cubes[0][y][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInRight);

            hasBlockInRight = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInRight == cube;
          }

          if (z - 1 >= 0)
          {
            int blockInBack = m_Cubes[x][y][z - 1];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInBack);

            hasBlockInBack = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInBack == cube;
          }
          else if (neighbors[3] != NULL)
          {
            int blockInBack = neighbors[3]->m_Cubes[x][y][WorldConstants::CHUNK_SIZE - 1];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInBack);

            hasBlockInBack = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInBack == cube;
          }

          if (x - 1 >= 0)
          {
            int blockInLeft = m_Cubes[x - 1][y][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInLeft);

            hasBlockInLeft = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInLeft == cube;
          }
          else if (neighbors[0] != NULL)
          {
            int blockInLeft = neighbors[0]->m_Cubes[WorldConstants::CHUNK_SIZE - 1][y][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInLeft);

            hasBlockInLeft = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInLeft == cube;
          }

          if (y + 1 < WorldConstants::CHUNK_HEIGHT)
          {
            int blockInTop = m_Cubes[x][y + 1][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInTop);

            hasBlockInTop = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInTop == cube;
          }

          if (y - 1 >= 0)
          {
            int blockInBottom = m_Cubes[x][y - 1][z];

            BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(blockInBottom);

            hasBlockInBottom = cubeInfo.isOpaque ? blockInfo.isOpaque : blockInfo.isOpaque || blockInBottom == cube;
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

void Chunk::Draw(Shader *shader)
{
  shader->SetUniform1i("uIsOpaque", 1);

  m_VAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_MeshVertexCount);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader->SetUniform1i("uIsOpaque", 0);

  m_TransparentVAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_TransparentMeshVertexCount);

  glDisable(GL_BLEND);
}
