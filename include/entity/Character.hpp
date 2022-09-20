#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <functional>
#include <glm/glm.hpp>

#include "core/matrices.hpp"

#include "entity/Camera.hpp"
#include "entity/Input.hpp"
#include "entity/Window.hpp"
#include "entity/UserInterface.hpp"

#include "world/World.hpp"

#include "physics/Ray.hpp"

const int HOTBAR_SIZE = 9;

class Character
{
private:
  const float BREAK_COOLDOWN = 0.2f;
  const float BASE_SPEED = 5.0f;
  const float RUN_SPEED = 100.0f;

  const float GRAVITY = 28.0f;
  const float TERMINAL_FALLING_SPEED = 60.0f;

  const float JUMP_COOLDOWN = 0.2f;
  const float CHARACTER_HEIGHT = 1.8f;

  glm::vec4 m_Position;

  bool m_ShouldBreakBlock = false;
  bool m_ShouldPlaceBlock = false;
  bool m_ShouldPickBlock = false;

  float m_FallingTime = 0.0f;
  bool m_IsOnGround = false;

  bool m_UseFreeControls = false;

  std::array<int, HOTBAR_SIZE> m_Hotbar = {};
  int m_HotbarPosition = 0;

public:
  Character();
  Character(glm::vec4 position);
  ~Character();

  void SetPosition(glm::vec4 position);

  glm::vec4 GetPosition() const { return m_Position; }

  void UseFreeControls() { m_UseFreeControls = true; }
  void UseNormalControls() { m_UseFreeControls = false; }

  void SetHotbarItem(int position, int id);

  int GetHotbarPosition() const { return m_HotbarPosition; }
  std::array<int, HOTBAR_SIZE> GetHotbar() const { return m_Hotbar; }

  void Update(Camera *camera, World *world);

  void OnClick(int button, int action, int mods);
  void OnKeypress(int key, int scancode, int action, int mods);
  void OnScroll(double xoffset, double yoffset);
};

#endif