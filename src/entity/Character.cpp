#include "entity/Character.hpp"

#include <cstdio>

Character::Character(Shader *shader, glm::vec4 position)
    : m_Position(position),
      m_Shader(shader)
{
  for (int i = 0; i < HOTBAR_SIZE; i++)
  {
    SetHotbarItem(i, i + 1);
  }

  m_JumpCurve = new Bezier(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, JUMP_HEIGHT / 2), glm::vec2(0.0f, JUMP_HEIGHT), glm::vec2(1.0f, JUMP_HEIGHT));

  float minX = -0.3f;
  float maxX = 0.3f;

  float minY = 0.0f;
  float maxY = 1.8f;

  float minZ = -0.3f;
  float maxZ = 0.3f;

  float vertices[] = {
      minX, maxY, maxZ, 1.0f, // 0
      minX, minY, maxZ, 1.0f, // 1
      maxX, minY, maxZ, 1.0f, // 2
      maxX, maxY, maxZ, 1.0f, // 3

      minX, maxY, minZ, 1.0f, // 4
      minX, minY, minZ, 1.0f, // 5
      maxX, minY, minZ, 1.0f, // 6
      maxX, maxY, minZ, 1.0f, // 7
  };

  m_VAO = new VertexArray();

  m_VBO = new VertexBuffer(vertices, 8 * 4 * sizeof(float));

  std::array<unsigned int, 3 * 2 * 6> indices = {
      0, 1, 2, // triângulo 1
      7, 6, 5, // triângulo 2
      3, 2, 6, // triângulo 3
      4, 0, 3, // triângulo 4
      4, 5, 1, // triângulo 5
      1, 5, 6, // triângulo 6
      0, 2, 3, // triângulo 7
      7, 5, 4, // triângulo 8
      3, 6, 7, // triângulo 9
      4, 3, 7, // triângulo 10
      4, 1, 0, // triângulo 11
      1, 6, 2, // triângulo 12
  };

  VertexBufferLayout layout;
  layout.Push(LayoutType::LT_FLOAT, 4);
  m_VAO->AddBuffer(*m_VBO, layout);

  m_IB = new IndexBuffer(indices.data(), indices.size());
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
      float distanceJumped = m_JumpCurve->GetPoint(m_JumpingTime / JUMP_TIME).y;

      m_JumpingTime += Window::GetDeltaTime();

      if (m_JumpingTime >= JUMP_TIME)
      {
        m_IsJumping = false;
        m_JumpingTime = 0.0f;
      }

      float currentJumpDistance = m_JumpCurve->GetPoint(m_JumpingTime / JUMP_TIME).y;

      verticalSpeed = (currentJumpDistance - distanceJumped) / Window::GetDeltaTime();

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

void Character::Draw(glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  glm::mat4 model = Matrices::MatrixTranslate(0.0f, 30.0f, 0.0f);

  m_Shader->SetUniformMat4f("uTransform", model);

  m_VAO->Bind();
  m_IB->Bind();

  glDrawElements(GL_TRIANGLES, m_IB->GetCount(), GL_UNSIGNED_INT, nullptr);
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
