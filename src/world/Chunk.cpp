#include <glm/glm.hpp>
#include <glad/glad.h>

#include "world/Chunk.hpp"

#include "world/BlockDatabase.hpp"

Chunk::Chunk()
    : m_VAO(NULL),
      m_VBO(NULL)
{
  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        m_Cubes[x][y][z] = y == CHUNK_HEIGHT - 1 ? GRASS : DIRT;
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

  std::vector<CubeVertex> vertices;

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
            hasBlockInFront = m_Cubes[x][y][z + 1] != 0;
          }
          else if (neighbors[1] != NULL)
          {
            hasBlockInFront = neighbors[1]->m_Cubes[x][y][0] != 0;
          }

          if (x + 1 < CHUNK_SIZE)
          {
            hasBlockInRight = m_Cubes[x + 1][y][z] != 0;
          }
          else if (neighbors[2] != NULL)
          {
            hasBlockInRight = neighbors[2]->m_Cubes[0][y][z] != 0;
          }

          if (z - 1 >= 0)
          {
            hasBlockInBack = m_Cubes[x][y][z - 1] != 0;
          }
          else if (neighbors[3] != NULL)
          {
            hasBlockInBack = neighbors[3]->m_Cubes[x][y][CHUNK_SIZE - 1] != 0;
          }

          if (x - 1 >= 0)
          {
            hasBlockInLeft = m_Cubes[x - 1][y][z] != 0;
          }
          else if (neighbors[0] != NULL)
          {
            hasBlockInLeft = neighbors[0]->m_Cubes[CHUNK_SIZE - 1][y][z] != 0;
          }

          if (y + 1 < CHUNK_HEIGHT)
          {
            hasBlockInTop = m_Cubes[x][y + 1][z] != 0;
          }

          if (y - 1 >= 0)
          {
            hasBlockInBottom = m_Cubes[x][y - 1][z] != 0;
          }

          std::array<bool, 6> occlusion = {
              hasBlockInFront,
              hasBlockInRight,
              hasBlockInBack,
              hasBlockInLeft,
              hasBlockInTop,
              hasBlockInBottom};

          BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(cube);

          std::vector<CubeVertex> visibleVertices = Cube::GetVisibleVertices(glm::vec3(x, y, z), blockInfo.textureCoordinates, occlusion);
          vertices.insert(vertices.end(), visibleVertices.begin(), visibleVertices.end());
        }
      }
    }
  }

  if (m_VBO != NULL)
    delete m_VBO;

  m_VBO = new VertexBuffer(&vertices[0], vertices.size() * sizeof(CubeVertex));

  VertexBufferLayout *layout = new VertexBufferLayout();

  layout->Push(LayoutType::LT_FLOAT, 4);
  layout->Push(LayoutType::LT_FLOAT, 2);
  layout->Push(LayoutType::LT_FLOAT, 4);

  m_VAO->AddBuffer(*m_VBO, *layout);

  m_MeshVertexCount = vertices.size();
}

void Chunk::Draw()
{
  m_VAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_MeshVertexCount);
}
