#include "core.h"

#include "world/BlockDatabase.hpp"
#include "world/World.hpp"
#include "world/WorldConstants.hpp"

namespace Collisions
{
  bool aabbWorldCollision(glm::vec3 playerPosition, glm::vec3 playerSize, World *world);
}