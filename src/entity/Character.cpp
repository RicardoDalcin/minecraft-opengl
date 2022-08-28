#include "entity/Character.hpp"

#include <cstdio>

Character::Character()
    : m_Position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Character::Character(glm::vec4 position)
    : m_Position(position)
{
}

Character::~Character()
{
}

void Character::SetPosition(glm::vec4 position)
{
  m_Position = position;
}

void Character::Update(Camera *camera, World *world)
{
  glm::vec2 deltaPos = Input::getDeltaMousePosition();

  float newTheta = camera->getCameraTheta() - 0.003f * deltaPos.x;
  float newPhi = camera->getCameraPhi() - 0.003f * deltaPos.y;

  // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
  float phimax = 3.141592f / 2;
  float phimin = -phimax;

  if (newPhi > phimax)
    newPhi = phimax;

  if (newPhi < phimin)
    newPhi = phimin;

  camera->updateCameraAngles(newTheta, newPhi);
  Input::resetDeltas();

  float baseSpeed = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? m_RunSpeed : m_BaseSpeed;

  float speed = baseSpeed * Window::GetDeltaTime();

  glm::vec4 newCameraPosition = camera->getPosition();

  if (Input::isKeyPressed(GLFW_KEY_W))
  {
    newCameraPosition += camera->getTarget() * speed;
  }

  if (Input::isKeyPressed(GLFW_KEY_S))
  {
    newCameraPosition -= camera->getTarget() * speed;
  }

  if (Input::isKeyPressed(GLFW_KEY_A))
  {
    newCameraPosition -= camera->getRight() * speed;
  }

  if (Input::isKeyPressed(GLFW_KEY_D))
  {
    newCameraPosition += camera->getRight() * speed;
  }

  camera->updatePosition(newCameraPosition);

  Ray ray(5.5f);

  glm::vec3 pos;
  glm::vec3 dir;

  m_BreakBlockTimer += Window::GetDeltaTime();

  if (m_BreakBlockTimer > BREAK_COOLDOWN)
  {
    m_CanBreakBlock = true;
  }

  if (ray.RayCast(camera->getPosition(), camera->getTarget(), world, World::RayCastCallback, &pos, &dir))
  {
    printf("Raycast hit at (%f, %f, %f)\n", pos.x, pos.y, pos.z);

    if (m_CanBreakBlock)
    {
      if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
      {
        world->SetBlock(pos, AIR);
        m_BreakBlockTimer = 0.0f;
        m_CanBreakBlock = false;
      }
      else if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
      {
        world->SetBlock(pos + dir, COBBLESTONE);
        m_BreakBlockTimer = 0.0f;
        m_CanBreakBlock = false;
      }
    }
  }
}