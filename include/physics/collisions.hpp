#include "core.h"

#include "world/BlockDatabase.hpp"
#include "world/World.hpp"
#include "world/WorldConstants.hpp"

namespace Collisions
{
  int Sign(float x);
  glm::vec3 Intbound(glm::vec3 point, glm::vec3 direction);

  bool BoundingBoxWorldCollision(glm::vec3 entityPosition, glm::vec3 entitySize, World *world);
  bool RayCast(float length, glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec4), glm::vec3 *out, glm::vec3 *directionOut);
}