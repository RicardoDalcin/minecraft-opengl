#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "glm/glm.hpp"

#include "entity/Camera.hpp"
#include "entity/Input.hpp"
#include "entity/Window.hpp"

class Character
{
private:
  const float m_BaseSpeed = 2.5f;
  const float m_RunSpeed = 100.0f;

  glm::vec4 m_Position;

public:
  Character();
  Character(glm::vec4 position);
  ~Character();

  void SetPosition(glm::vec4 position);

  glm::vec4 GetPosition() const { return m_Position; }

  void Update(Camera *camera);
};

#endif