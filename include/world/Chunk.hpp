#ifndef _CHUNK_H
#define _CHUNK_H

#include <vector>

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "world/Cube.hpp"

class Chunk
{
public:
  static const int CHUNK_SIZE = 16;
  static const int CHUNK_HEIGHT = 64;

private:
  int m_ChunkX;
  int m_ChunkZ;

  VertexArray *m_VAO;
  VertexBuffer *m_VBO;

  int m_MeshVertexCount;

  VertexArray *m_TransparentVAO;
  VertexBuffer *m_TransparentVBO;

  int m_TransparentMeshVertexCount;

  std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_HEIGHT>, CHUNK_SIZE> m_Cubes;

public:
  Chunk(int chunkX, int chunkZ);
  ~Chunk();

  int GetChunkX() const { return m_ChunkX; }
  int GetChunkZ() const { return m_ChunkZ; }

  void BuildMesh(std::array<Chunk *, 4> neighbors);

  void Draw();
};

#endif