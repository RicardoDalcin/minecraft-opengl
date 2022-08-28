#include "physics/Ray.hpp"

class World;

Ray::Ray(float length)
    : m_Length(length)
{
}

Ray::~Ray()
{
}

int sign(float x)
{
  return x > 0 ? 1 : x < 0 ? -1
                           : 0;
}

glm::vec3 intbound(glm::vec3 s, glm::vec3 ds)
{
  glm::vec3 v;

  for (size_t i = 0; i < 3; i++)
  {
    v[i] = (ds[i] > 0 ? (ceilf(s[i]) - s[i]) : (s[i] - floorf(s[i]))) / fabsf(ds[i]);
  }
  return v;
}

bool Ray::RayCast(glm::vec4 origin, glm::vec4 direction, World *data, bool (*callback)(World *, glm::vec3), glm::vec3 *out, glm::vec3 *directionOut)
{
  glm::vec3 p = glm::vec3(floorf(origin.x), floorf(origin.y), floorf(origin.z));

  glm::vec3 step = glm::vec3(sign(direction.x), sign(direction.y), sign(direction.z));

  glm::vec3 tmax = intbound(origin, direction);

  glm::vec3 tdelta = glm::vec3(step.x / direction.x, step.y / direction.y, step.z / direction.z);

  float radius = m_Length / norm(glm::vec4(direction.x, direction.y, direction.z, 0.0f));

  while (true)
  {
    if (callback(data, p))
    {
      *out = p;
      return true;
    }

    if (tmax.x < tmax.y)
    {
      if (tmax.x < tmax.z)
      {
        if (tmax.x > radius)
        {
          break;
        }

        p.x += step.x;
        tmax.x += tdelta.x;
        *directionOut = glm::vec3(-step.x, 0.0f, 0.0f);
      }
      else
      {
        if (tmax.z > radius)
        {
          break;
        }

        p.z += step.z;
        tmax.z += tdelta.z;
        *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
      }
    }
    else
    {
      if (tmax.y < tmax.z)
      {
        if (tmax.y > radius)
        {
          break;
        }

        p.y += step.y;
        tmax.y += tdelta.y;
        *directionOut = glm::vec3(0.0f, -step.y, 0.0f);
      }
      else
      {
        if (tmax.z > radius)
        {
          break;
        }

        p.z += step.z;
        tmax.z += tdelta.z;
        *directionOut = glm::vec3(0.0f, 0.0f, -step.z);
      }
    }
  }

  return false;
}