#ifndef SQUARE_PLAYER_H
#define SQUARE_PLAYER_H

namespace MinecraftClone
{
  struct Window;

  namespace Cubes
  {
    void init(Shader shader);
    void destroy();

    void update(float dt, Shader shader, glm::mat4 view, glm::mat4 proj);
  }
}

#endif