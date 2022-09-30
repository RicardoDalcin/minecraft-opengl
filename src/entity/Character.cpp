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

  // Icon texture is the face, but leaves use the top texture
  int offset = blockInfo.blockId.find("_leaves") != std::string::npos ? 24 : 0;

  std::array<glm::vec2, 4> faceCoords = {
      blockInfo.textureCoordinates[offset + 0],
      blockInfo.textureCoordinates[offset + 1],
      blockInfo.textureCoordinates[offset + 2],
      blockInfo.textureCoordinates[offset + 3]};

  UserInterface::UpdateHotbarPosition(position, faceCoords);
}

void Character::Update(Camera *camera, World *world)
{
  glm::vec2 deltaPos = Input::GetDeltaMousePosition();

  float newTheta = camera->GetCameraTheta() - 0.003f * deltaPos.x;
  float newPhi = camera->GetCameraPhi() - 0.003f * deltaPos.y;

  // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
  float phimax = 3.141592f / 2;
  float phimin = -phimax;

  if (newPhi > phimax)
    newPhi = phimax;

  if (newPhi < phimin)
    newPhi = phimin;

  // Process new angles and reset deltas
  camera->UpdateCameraAngles(newTheta, newPhi);
  Input::ResetDeltas();

  if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) && Input::IsKeyPressed(GLFW_KEY_W))
  {
    camera->SetFOV(75.0f);
  }
  else
  {
    camera->SetFOV(70.0f);
  }

  float baseSpeed = Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)
                        ? (m_UseFreeControls ? FLYING_SPEED : RUNNING_SPEED)
                        : BASE_SPEED;

  float speed = baseSpeed * Window::GetDeltaTime();

  glm::vec4 newCameraPosition = camera->GetPosition();

  if (Input::IsKeyPressed(GLFW_KEY_W))
  {
    glm::vec4 cameraTarget = camera->GetTarget();

    if (m_UseFreeControls)
    {
      newCameraPosition += cameraTarget * speed;
    }
    else
    {
      glm::vec4 normalizedTarget = glm::vec4(cameraTarget.x, 0.0f, cameraTarget.z, 0.0f);
      normalizedTarget = normalizedTarget / Matrices::Norm(normalizedTarget);

      newCameraPosition += normalizedTarget * speed;
    }
  }

  if (Input::IsKeyPressed(GLFW_KEY_S))
  {
    glm::vec4 cameraTarget = camera->GetTarget();

    if (m_UseFreeControls)
    {
      newCameraPosition -= cameraTarget * speed;
    }
    else
    {
      glm::vec4 normalizedTarget = glm::vec4(cameraTarget.x, 0.0f, cameraTarget.z, 0.0f);
      normalizedTarget = normalizedTarget / Matrices::Norm(normalizedTarget);

      newCameraPosition -= normalizedTarget * speed;
    }
  }

  if (Input::IsKeyPressed(GLFW_KEY_A))
  {
    newCameraPosition -= camera->GetRight() * speed;
  }

  if (Input::IsKeyPressed(GLFW_KEY_D))
  {
    newCameraPosition += camera->GetRight() * speed;
  }

  if (m_UseFreeControls)
  {
    camera->UpdatePosition(newCameraPosition);
  }
  else
  {
    if (Collisions::BoundingBoxWorldCollision(newCameraPosition, glm::vec3(0.6f, 1.8f, 0.6f), world))
    {
      newCameraPosition = camera->GetPosition();
    }
  }

  glm::vec3 pos;
  glm::vec3 dir;

  if (Collisions::RayCast(5.5f, newCameraPosition, camera->GetTarget(), world, World::RayCastCallback, &pos, &dir))
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
      int currentBlock = world->GetBlock(pos + dir);

      if (currentBlock == AIR || currentBlock == WATER)
      {
        world->SetBlock(pos + dir, block);

        if (!m_UseFreeControls)
        {
          if (Collisions::BoundingBoxWorldCollision(newCameraPosition, glm::vec3(0.6f, 1.8f, 0.6f), world))
          {
            world->SetBlock(pos + dir, currentBlock);
          }
        }
      }
    }
  }

  m_ShouldBreakBlock = false;
  m_ShouldPlaceBlock = false;
  m_ShouldPickBlock = false;

  glm::vec3 fallingPos;
  glm::vec3 fallingDir;

  // FIXME: infinite loop when either x or z are 0.0f
  glm::vec4 fallingDirection = glm::vec4(0.0001f, -1.0f, 0.0001f, 0.0f);

  if (!m_UseFreeControls)
  {
    float verticalSpeed = 0.0f;

    if (Input::IsKeyPressed(GLFW_KEY_SPACE))
    {
      if (m_IsOnGround && !m_IsJumping)
      {
        m_IsOnGround = false;
        m_FallingTime = 0.0f;

        m_IsJumping = true;
        m_JumpingTime = 0.0f;
      }
    }

    if (m_IsJumping)
    {
      float distanceJumped = JUMP_INITIAL_SPEED * m_JumpingTime + (JUMP_FORCE * m_JumpingTime * m_JumpingTime) / 2;

      if (distanceJumped >= JUMP_HEIGHT)
      {
        m_IsJumping = false;
        m_JumpingTime = 0.0f;
      }
      else
      {
        m_JumpingTime += Window::GetDeltaTime();
      }

      verticalSpeed = JUMP_INITIAL_SPEED + JUMP_FORCE * m_JumpingTime;

      if (verticalSpeed < 0.05f)
        verticalSpeed = 0.05f;

      glm::vec4 position = newCameraPosition + glm::vec4(0.0f, verticalSpeed * Window::GetDeltaTime() + 0.1f, 0.0f, 0.0f);

      if (Collisions::PointWorldCollision(position, world))
      {
        m_IsJumping = false;
        m_JumpingTime = 0.0f;
      }
    }

    if (!Collisions::RayCast(CHARACTER_HEIGHT, newCameraPosition, fallingDirection, world, World::RayCastCallback, &fallingPos, &fallingDir))
    {
      if (!m_IsJumping)
      {
        if (m_IsOnGround)
        {
          m_IsOnGround = false;
          m_FallingTime = 0.0f;
        }
        else
        {
          m_FallingTime += Window::GetDeltaTime();
        }

        verticalSpeed = GRAVITY * m_FallingTime;

        if (verticalSpeed > TERMINAL_FALLING_SPEED)
          verticalSpeed = TERMINAL_FALLING_SPEED;

        verticalSpeed *= -1;
      }
    }
    else
    {
      if (!m_IsOnGround && !m_IsJumping)
      {
        m_IsOnGround = true;
        m_FallingTime = 0.0f;
      }
    }

    camera->UpdatePosition(newCameraPosition + glm::vec4(0.0f, verticalSpeed * Window::GetDeltaTime(), 0.0f, 0.0f));
  }
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

void Character::OnKeypress(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_F && action == GLFW_PRESS)
  {
    UseFreeControls();
  }
  else if (key == GLFW_KEY_G && action == GLFW_PRESS)
  {
    UseNormalControls();
  }

  if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS)
  {
    m_HotbarPosition = key - GLFW_KEY_1;
  }
}

void Character::OnScroll(double xoffset, double yoffset)
{
  if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
  {
    int newBlock = m_Hotbar[m_HotbarPosition] - (int)yoffset;

    if (newBlock < 1)
    {
      newBlock = BLOCK_COUNT - 1;
    }
    else if (newBlock > BLOCK_COUNT - 1)
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