#include <glm/glm.hpp>
#include <glad/glad.h>

#include "world/Chunk.hpp"

#include "world/BlockDatabase.hpp"

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX),
      m_ChunkZ(chunkZ),
      m_VAO(NULL),
      m_VBO(NULL),
      m_TransparentVAO(NULL),
      m_TransparentVBO(NULL)
{
  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        m_Cubes[x][y][z] = y == CHUNK_HEIGHT - 1 ? WATER : DIRT;
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

  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
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

          if (z + 1 < CHUNK_SIZE)
          {
            int blockInFront = m_Cubes[x][y][z + 1];
            hasBlockInFront = blockInFront != AIR && blockInFront != WATER;
          }
          else if (neighbors[1] != NULL)
          {
            int blockInFront = neighbors[1]->m_Cubes[x][y][0];
            hasBlockInFront = blockInFront != AIR && blockInFront != WATER;
          }

          if (x + 1 < CHUNK_SIZE)
          {
            int blockInRight = m_Cubes[x + 1][y][z];
            hasBlockInRight = blockInRight != AIR && blockInRight != WATER;
          }
          else if (neighbors[2] != NULL)
          {
            int blockInRight = neighbors[2]->m_Cubes[0][y][z];
            hasBlockInRight = blockInRight != AIR && blockInRight != WATER;
          }

          if (z - 1 >= 0)
          {
            int blockInBack = m_Cubes[x][y][z - 1];
            hasBlockInBack = blockInBack != AIR && blockInBack != WATER;
          }
          else if (neighbors[3] != NULL)
          {
            int blockInBack = neighbors[3]->m_Cubes[x][y][CHUNK_SIZE - 1];
            hasBlockInBack = blockInBack != AIR && blockInBack != WATER;
          }

          if (x - 1 >= 0)
          {
            int blockInLeft = m_Cubes[x - 1][y][z];
            hasBlockInLeft = blockInLeft != AIR && blockInLeft != WATER;
          }
          else if (neighbors[0] != NULL)
          {
            int blockInLeft = neighbors[0]->m_Cubes[CHUNK_SIZE - 1][y][z];
            hasBlockInLeft = blockInLeft != AIR && blockInLeft != WATER;
          }

          if (y + 1 < CHUNK_HEIGHT)
          {
            int blockInTop = m_Cubes[x][y + 1][z];
            hasBlockInTop = blockInTop != AIR && blockInTop != WATER;
          }

          if (y - 1 >= 0)
          {
            int blockInBottom = m_Cubes[x][y - 1][z];
            hasBlockInBottom = blockInBottom != AIR && blockInBottom != WATER;
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
