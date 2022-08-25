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
        int randomBlockIndex = rand() % 50;

        BlockInformation randomBlock = BlockDatabase::GetBlockInformationIndex(randomBlockIndex);
        // BlockInformation randomBlock = BlockDatabase::GetBlockInformation(y == CHUNK_HEIGHT - 1 ? "grass" : "dirt");

        m_Cubes[x][y][z] = new Cube(glm::vec4(x, y, z, 1.0f), randomBlock.textureReference, randomBlock.textureCoordinates);
      }
    }
  }

  BuildMesh();
}

Chunk::~Chunk()
{
  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        delete m_Cubes[x][y][z];
      }
    }
  }
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
        Cube *cube = m_Cubes[x][y][z];

        if (cube != NULL)
        {
          bool hasBlockInFront = z + 1 < CHUNK_SIZE && m_Cubes[x][y][z + 1] != NULL;
          bool hasBlockInRight = x + 1 < CHUNK_SIZE && m_Cubes[x + 1][y][z] != NULL;
          bool hasBlockInBack = z - 1 >= 0 && m_Cubes[x][y][z - 1] != NULL;
          bool hasBlockInLeft = x - 1 >= 0 && m_Cubes[x - 1][y][z] != NULL;
          bool hasBlockInTop = y + 1 < CHUNK_HEIGHT && m_Cubes[x][y + 1][z] != NULL;
          bool hasBlockInBottom = y - 1 >= 0 && m_Cubes[x][y - 1][z] != NULL;

          std::array<bool, 6> occlusion = {
              hasBlockInFront,
              hasBlockInRight,
              hasBlockInBack,
              hasBlockInLeft,
              hasBlockInTop,
              hasBlockInBottom};

          std::vector<CubeVertex> visibleVertices = cube->GetVisibleVertices(occlusion);
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
