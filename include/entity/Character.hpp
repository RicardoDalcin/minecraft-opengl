#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <functional>
#include <glm/glm.hpp>

#include "entity/Camera.hpp"
#include "entity/Input.hpp"
#include "entity/Window.hpp"

#include "world/World.hpp"

#include "physics/Ray.hpp"

const int HOTBAR_SIZE = 9;

class Character
{
private:
  const float m_BaseSpeed = 2.5f;
  const float m_RunSpeed = 100.0f;

  glm::vec4 m_Position;

  const float BREAK_COOLDOWN = 0.2f;

  bool m_ShouldBreakBlock = false;
  bool m_ShouldPlaceBlock = false;
  bool m_ShouldPickBlock = false;

  int m_BlockToPlace = 1;

  int m_HotbarPosition = 0;

public:
  Character();
  Character(glm::vec4 position);
  ~Character();

  void SetPosition(glm::vec4 position);

  glm::vec4 GetPosition() const { return m_Position; }
  int GetHotbarPosition() const { return m_HotbarPosition; }

  void Update(Camera *camera, World *world);

  void OnClick(int button, int action, int mods);
  void OnScroll(double xoffset, double yoffset);
};

#endif