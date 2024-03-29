#ifndef _CHUNK_H
#define _CHUNK_H

#include <vector>

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/VertexBufferLayout.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "world/Cube.hpp"
#include "world/WorldConstants.hpp"

// Classe para representação de um chunk
class Chunk
{
private:
  int m_ChunkX;
  int m_ChunkZ;

  VertexArray *m_VAO;
  VertexBuffer *m_VBO;

  int m_MeshVertexCount;

  VertexArray *m_TransparentVAO;
  VertexBuffer *m_TransparentVBO;

  int m_TransparentMeshVertexCount;

  std::array<std::array<std::array<int, WorldConstants::CHUNK_SIZE>, WorldConstants::CHUNK_HEIGHT>, WorldConstants::CHUNK_SIZE> m_Cubes;

public:
  Chunk(int chunkX, int chunkZ);
  ~Chunk();

  int GetChunkX() const { return m_ChunkX; }
  int GetChunkZ() const { return m_ChunkZ; }

  int GetCube(glm::vec3 position) const { return m_Cubes[position.x][position.y][position.z]; }
  void SetCube(glm::vec3 position, int block) { m_Cubes[position.x][position.y][position.z] = block; }

  void BuildMesh(std::array<Chunk *, 4> neighbors);

  void Draw(Shader *shader);
};

#endif