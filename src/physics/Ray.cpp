#include "physics/Ray.hpp"

class World;

Ray::Ray(float length)
    : m_Length(length)
{
}

Ray::~Ray()
{
}

int Ray::Sign(float x)
{
  return x > 0 ? 1 : x < 0 ? -1
                           : 0;
}

glm::vec3 Ray::Intbound(glm::vec3 point, glm::vec3 direction)
{
  glm::vec3 bound;

  for (size_t i = 0; i < 3; i++)
  {
    bound[i] = (direction[i] > 0 ? (ceilf(point[i]) - point[i]) : (point[i] - floorf(point[i]))) / fabsf(direction[i]);
  }

  return bound;
}

bool Ray::RayCast(glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec4), glm::vec3 *out, glm::vec3 *directionOut)
{
  glm::vec3 step = glm::vec3(Sign(direction.x), Sign(direction.y), Sign(direction.z));

  glm::vec3 rayMax = Intbound(origin, direction);

  glm::vec3 rayDelta = glm::vec3(step.x / direction.x, step.y / direction.y, step.z / direction.z);

  float radius = m_Length / Matrices::Norm(direction);

  while (true)
  {
    if (callback(data, origin))
    {
      *out = origin;
      return true;
    }

    if (rayMax.x < rayMax.y)
    {
      if (rayMax.x < rayMax.z)
      {
        if (rayMax.x > radius)
        {
          break;
        }

        origin.x += step.x;
        rayMax.x += rayDelta.x;
        *directionOut = glm::vec3(-step.x, 0.0f, 0.0f);
      }
      else
      {
        if (rayMax.z > radius)
        {
          break;
        }

        origin.z += step.z;
        rayMax.z += rayDelta.z;
        *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
      }
    }
    else
    {
      if (rayMax.y < rayMax.z)
      {
        if (rayMax.y > radius)
        {
          break;
        }

        origin.y += step.y;
        rayMax.y += rayDelta.y;
        *directionOut = glm::vec3(0.0f, -step.y, 0.0f);
      }
      else
      {
        if (rayMax.z > radius)
        {
          break;
        }

        origin.z += step.z;
        rayMax.z += rayDelta.z;
        *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
      }
    }
  }

  return false;
}