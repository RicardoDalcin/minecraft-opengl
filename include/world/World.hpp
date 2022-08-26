#ifndef _WORLD_H
#define _WORLD_H

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "entity/Camera.hpp"

#include "world/Chunk.hpp"

class World
{
private:
  static const int CHUNKS_PER_AXIS = 8;

  Shader *m_Shader;
  Texture *m_TextureAtlas;

  std::array<std::array<Chunk *, CHUNKS_PER_AXIS>, CHUNKS_PER_AXIS> m_Chunks;

public:
  World(Shader *shader);
  ~World();

  void UpdateMeshes();
  void Draw(Camera *camera, glm::mat4 view, glm::mat4 projection);
};

#endif