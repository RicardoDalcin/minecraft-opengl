#include "entity/Character.hpp"

#include <cstdio>

// Classe para gerenciamento do personagem
Character::Character(Shader *shader, glm::vec4 position)
    : m_Shader(shader),
      m_Position(position)
{
  // Inicializa a hotbar com os 9 primeiros blocos
  for (int i = 0; i < HOTBAR_SIZE; i++)
  {
    SetHotbarItem(i, i + 1);
  }

  // Inicializa curva de pulo
  m_JumpCurve = new Bezier(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, JUMP_HEIGHT / 2), glm::vec2(0.0f, JUMP_HEIGHT), glm::vec2(1.0f, JUMP_HEIGHT));

  // Cria vértices e índices para renderização do model do personagem
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

// Atualiza uma posição da hotbar e atualiza os vértices para renderização da UI
void Character::SetHotbarItem(int position, int id)
{
  m_Hotbar[position] = id;

  BlockInformation blockInfo = BlockDatabase::GetBlockInformationIndex(id);

  // Textura do ícone é a face, porém para as folhas é o lado
  int offset = blockInfo.blockId.find("_leaves") != std::string::npos ? 24 : 0;

  std::array<glm::vec2, 4> faceCoords = {
      blockInfo.textureCoordinates[offset + 0],
      blockInfo.textureCoordinates[offset + 1],
      blockInfo.textureCoordinates[offset + 2],
      blockInfo.textureCoordinates[offset + 3]};

  UserInterface::UpdateHotbarPosition(position, faceCoords);
}

// Função de atualização do personagem que é chamada a cada frame
void Character::Update(Camera *camera, World *world)
{
  // Atualiza ângulos da câmera com base no delta da posição do mouse
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

  // Atualiza os novos ângulos na câmera e reseta os deltas
  camera->UpdateCameraAngles(newTheta, newPhi);
  Input::ResetDeltas();

  // Se estiver correndo, aumenta o FOV
  if (Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) && Input::IsKeyPressed(GLFW_KEY_W))
  {
    camera->SetFOV(75.0f);
  }
  else
  {
    camera->SetFOV(70.0f);
  }

  // Calcula velocidade do personagem dependendo do modo de controle
  float baseSpeed = Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)
                        ? (m_UseFreeControls ? FLYING_SPEED : RUNNING_SPEED)
                        : BASE_SPEED;

  float speed = baseSpeed * Window::GetDeltaTime();

  glm::vec4 newCameraPosition = camera->GetPosition();

  // Se estiver usando os controles livres, anda na direção da câmera, senão anda na horizontal
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

  // Se estiver usando os controles livres, anda na direção contrária da câmera, senão anda na horizontal
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

  // Comportamento para A e D é o mesmo em ambos modos de controle
  if (Input::IsKeyPressed(GLFW_KEY_A))
  {
    newCameraPosition -= camera->GetRight() * speed;
  }

  if (Input::IsKeyPressed(GLFW_KEY_D))
  {
    newCameraPosition += camera->GetRight() * speed;
  }

  // Se estiver usando os controles livres atualiza a posição, senão verifica a colisão antes de atualizar
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

  // Testa se está "mirando" em algum bloco na distância
  if (Collisions::RayCast(5.5f, newCameraPosition, camera->GetTarget(), world, World::RayCastCallback, &pos, &dir))
  {
    // Faz block picking
    if (m_ShouldPickBlock)
    {
      SetHotbarItem(m_HotbarPosition, world->GetBlock(glm::vec3(pos.x, pos.y, pos.z)));
    }

    int block = m_Hotbar[m_HotbarPosition];

    // Coloca bloco
    if (m_ShouldBreakBlock)
    {
      world->SetBlock(pos, AIR);
    }
    // Quebra bloco se não for ar ou água
    else if (m_ShouldPlaceBlock && block != AIR && block != WATER)
    {
      int currentBlock = world->GetBlock(pos + dir);

      if (currentBlock == AIR || currentBlock == WATER)
      {
        // Atualiza o bloco na direção na qual a câmera "atinge" o bloco
        world->SetBlock(pos + dir, block);

        // Se não estiver usando controle livre, testa se o bloco colocado está colidindo com o personagem
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

  glm::vec4 fallingDirection = glm::vec4(0.0001f, -1.0f, 0.0001f, 0.0f);

  // Se estiver no modo com física, calcula a velocidade vertical do personagem com base em queda/pulo
  if (!m_UseFreeControls)
  {
    float verticalSpeed = 0.0f;

    // Se estiver no chão e apertar espaço, começa o pulo
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

    // Calcula a velocidade vertical atual do pulo com base na curva de bézier
    if (m_IsJumping)
    {
      float distanceJumped = m_JumpCurve->GetPoint(m_JumpingTime / JUMP_TIME).y;

      m_JumpingTime += Window::GetDeltaTime();

      // Se o tempo de pulo acabou, para o pulo
      if (m_JumpingTime >= JUMP_TIME)
      {
        m_IsJumping = false;
        m_JumpingTime = 0.0f;
      }

      float currentJumpDistance = m_JumpCurve->GetPoint(m_JumpingTime / JUMP_TIME).y;

      verticalSpeed = (currentJumpDistance - distanceJumped) / Window::GetDeltaTime();

      glm::vec4 position = newCameraPosition + glm::vec4(0.0f, verticalSpeed * Window::GetDeltaTime() + 0.1f, 0.0f, 0.0f);

      // Se estiver colidindo com o "teto", para o pulo
      if (Collisions::PointWorldCollision(position, world))
      {
        m_IsJumping = false;
        m_JumpingTime = 0.0f;
      }
    }

    // Testa se o personagem não está no chão
    if (!Collisions::RayCast(CHARACTER_HEIGHT, newCameraPosition, fallingDirection, world, World::RayCastCallback, &fallingPos, &fallingDir))
    {
      // Se não estiver no chão nem pulando, calcula a velocidade de queda com aceleração gravitacional
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

    // Atualiza a câmera com a nova posição computada junto com a velocidade vertical
    camera->UpdatePosition(newCameraPosition + glm::vec4(0.0f, verticalSpeed * Window::GetDeltaTime(), 0.0f, 0.0f));
  }
}

// Desenha o model do personagem na posição da câmera
void Character::Draw(Camera *camera, glm::mat4 view, glm::mat4 projection)
{
  m_Shader->Bind();

  m_Shader->SetUniformMat4f("uView", view);
  m_Shader->SetUniformMat4f("uProjection", projection);

  glm::mat4 model = Matrices::MatrixTranslate(camera->GetPosition().x, camera->GetPosition().y - CHARACTER_HEIGHT, camera->GetPosition().z);

  m_Shader->SetUniformMat4f("uTransform", model);

  m_VAO->Bind();
  m_IB->Bind();

  glDrawElements(GL_TRIANGLES, m_IB->GetCount(), GL_UNSIGNED_INT, nullptr);
}

// Gerencia o callback de click do mouse para o personagem
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

// Gerencia o callback de keypress para o personagem
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

// Gerencia o callback de scroll do mouse para o personagem
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
