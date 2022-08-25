#include <glm/glm.hpp>
#include <glad/glad.h>

#include "world/Chunk.hpp"

#include "world/BlockDatabase.hpp"

Chunk::Chunk(Shader *shader)
    : m_VAO(NULL),
      m_VBO(NULL),
      m_Shader(shader),
      m_TextureAtlas(new Texture("extras/textures/atlas.png", true))
{
  m_Shader->Bind();

  for (int x = 0; x < CHUNK_SIZE; x++)
  {
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        int randomBlockIndex = rand() % 50;

        BlockInformation randomBlock = BlockDatabase::GetBlockInformationIndex(randomBlockIndex);
        // BlockInformation randomBlock = BlockDatabase::GetBlockInformation(y == CHUNK_HEIGHT - 1 ? "grass" : "dirt");

        m_Cubes[x][y][z] =
            new Cube(
                glm::vec4(x, y, z, 1.0f),
                m_TextureAtlas,
                randomBlock.textureReference,
                m_Shader);
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

  delete m_TextureAtlas;
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
          glm::vec3 cubePosition = cube->GetPosition();

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

          std::array<CubeVertex, 36> cubeVertices = cube->GetVertices();

          for (int index = 0; index < 36; index++)
            if (!occlusion[floorf(index / 6)])
            {
              CubeVertex vertex = cubeVertices[index];
              CubeVertex transformedVertex = {
                  glm::vec4(cubePosition.x + vertex.position.x, cubePosition.y + vertex.position.y, cubePosition.z + vertex.position.z, 0.0f),
                  vertex.textureCoords,
                  vertex.normal,
              };
              vertices.push_back(transformedVertex);
            }
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

void Chunk::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // glm::mat4 transform = Matrix_Identity();
  m_Shader->SetUniformMat4f("uTransform", model);

  m_TextureAtlas->Bind(0);
  m_Shader->SetUniform1i("uTexture", 0);

  m_VAO->Bind();

  glDrawArrays(GL_TRIANGLES, 0, m_MeshVertexCount);
}
