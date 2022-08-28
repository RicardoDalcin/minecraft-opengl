#ifndef _RAY_H
#define _RAY_H

#include <functional>

#include <glm/glm.hpp>

#include "core/matrices.hpp"

#include "world/World.hpp"

class Ray
{
private:
  float m_Length;

public:
  Ray(float length);
  ~Ray();

  bool RayCast(glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec3), glm::vec3 *out, glm::vec3 *directionOut);
};

#endif