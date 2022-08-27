#ifndef _CHUNK_H
#define _CHUNK_H

#include <vector>

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "world/Cube.hpp"
#include "world/WorldConstants.hpp"

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

  void BuildMesh(std::array<Chunk *, 4> neighbors);

  void Draw();
};

#endif