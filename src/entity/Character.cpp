#include "entity/Character.hpp"

#include <cstdio>

Character::Character()
    : m_Position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Character::Character(glm::vec4 position)
    : m_Position(position)
{
  for (int i = 0; i < HOTBAR_SIZE; i++)
  {
    SetHotbarItem(i, i + 1);
  }
}

Character::~Character()
{
}

void Character::SetPosition(glm::vec4 position)
{
  m_Position = position;
}

void Character::SetHotbarItem(int position, int id)
{
  m_Hotbar[position] = id;

  BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(id);

  std::array<glm::vec2, 4> faceCoords = {
      blockInfo.textureCoordinates[0],
      blockInfo.textureCoordinates[1],
      blockInfo.textureCoordinates[2],
      blockInfo.textureCoordinates[3]};

  UserInterface::UpdateHotbarPosition(position, faceCoords);
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

  float baseSpeed = Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL) ? m_RunSpeed : m_BaseSpeed;

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

  if (ray.RayCast(camera->getPosition(), camera->getTarget(), world, World::RayCastCallback, &pos, &dir))
  {
    if (m_ShouldPickBlock)
    {
      SetHotbarItem(m_HotbarPosition, world->GetBlock(glm::vec3(pos.x, pos.y, pos.z)));
    }

    int block = m_Hotbar[m_HotbarPosition];

    if (m_ShouldBreakBlock)
    {
      world->SetBlock(pos, AIR);
    }
    else if (m_ShouldPlaceBlock && block != AIR && block != WATER)
    {
      if (world->GetBlock(pos + dir) == AIR)
      {
        world->SetBlock(pos + dir, block);
      }
    }
  }

  m_ShouldBreakBlock = false;
  m_ShouldPlaceBlock = false;
  m_ShouldPickBlock = false;
}

void Character::OnClick(int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    m_ShouldBreakBlock = true;
    m_ShouldPlaceBlock = false;
  }

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
  {
    m_ShouldPlaceBlock = true;
    m_ShouldBreakBlock = false;
  }

  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
  {
    m_ShouldPickBlock = true;
  }
}

void Character::OnScroll(double xoffset, double yoffset)
{
  if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
  {
    int newBlock = m_Hotbar[m_HotbarPosition] - (int)yoffset;

    if (newBlock < 1)
    {
      newBlock = 52;
    }
    else if (newBlock > 52)
    {
      newBlock = 1;
    }

    SetHotbarItem(m_HotbarPosition, newBlock);
  }
  else
  {
    int newHotbarPosition = m_HotbarPosition - (int)yoffset;
    m_HotbarPosition = newHotbarPosition < 0 ? HOTBAR_SIZE - 1 : newHotbarPosition % HOTBAR_SIZE;
  }
}