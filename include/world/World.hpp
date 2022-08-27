#ifndef _WORLD_H
#define _WORLD_H

#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "entity/Camera.hpp"

#include "world/Chunk.hpp"
#include "world/WorldConstants.hpp"

class World
{
private:
  Shader *m_Shader;
  Texture *m_TextureAtlas;

  std::array<std::array<Chunk *, WorldConstants::CHUNKS_PER_AXIS>, WorldConstants::CHUNKS_PER_AXIS> m_Chunks;

public:
  World(Shader *shader);
  ~World();

  void UpdateMeshes();
  void Draw(Camera *camera, glm::mat4 view, glm::mat4 projection);
};

#endif