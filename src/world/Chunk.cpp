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

  BuildMesh();
}

Chunk::~Chunk()
{
}

void Chunk::BuildMesh()
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
          bool hasBlockInFront = z + 1 < CHUNK_SIZE && m_Cubes[x][y][z + 1] != 0;
          bool hasBlockInRight = x + 1 < CHUNK_SIZE && m_Cubes[x + 1][y][z] != 0;
          bool hasBlockInBack = z - 1 >= 0 && m_Cubes[x][y][z - 1] != 0;
          bool hasBlockInLeft = x - 1 >= 0 && m_Cubes[x - 1][y][z] != 0;
          bool hasBlockInTop = y + 1 < CHUNK_HEIGHT && m_Cubes[x][y + 1][z] != 0;
          bool hasBlockInBottom = y - 1 >= 0 && m_Cubes[x][y - 1][z] != 0;

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
