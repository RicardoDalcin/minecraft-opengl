#include "entity/UserInterface.hpp"

const float UserInterface::crosshairWidth = 20.0f;
const float UserInterface::crosshairHeight = 20.0f;

const float UserInterface::crosshairCenterX = 0.0f;
const float UserInterface::crosshairCenterY = 0.0f;

const float UserInterface::hotbarSlotSize = 32.0f * 2.0f;

const float UserInterface::hotbarWidth = hotbarSlotSize * UI_HOTBAR_SIZE + 4.0f * 2.0f;
const float UserInterface::hotbarHeight = hotbarSlotSize;

const float UserInterface::hotbarCenterX = 0.0f;
const float UserInterface::hotbarCenterY = -0.9f;

const float UserInterface::hotbarSelectorWidth = 68.0f;
const float UserInterface::hotbarSelectorHeight = 68.0f;

const float UserInterface::hotbarIconWidth = 32.0f;
const float UserInterface::hotbarIconHeight = 32.0f;

std::array<float, 5 * 4 *UI_HOTBAR_SIZE> UserInterface::hotbarVertices = {};
std::array<unsigned int, 6 *UI_HOTBAR_SIZE> UserInterface::hotbarIndices = {};

void UserInterface::UpdateHotbarPosition(int position, std::array<glm::vec2, 4> textureCoords)
{
  if (position >= UI_HOTBAR_SIZE)
    return;

  int width = Window::GetWidth();
  int height = Window::GetHeight();

  float xUnit = (float)hotbarIconWidth / width;
  float yUnit = (float)hotbarIconHeight / height;

  float slotCenterX = (-(int)floor(UI_HOTBAR_SIZE / 2) * hotbarSlotSize * 2 + position * hotbarSlotSize * 2) / width;

  int hotbarVertexPosition = position * 20;
  float yCenter = hotbarCenterY + (16 / width);

  for (int i = 0; i < 4; i++)
  {
    float xUnitSigned = i == 0 || i == 3 ? xUnit : -xUnit;
    float yUnitSigned = i == 0 || i == 1 ? yUnit : -yUnit;

    hotbarVertices[hotbarVertexPosition + i * 5 + 0] = slotCenterX + xUnitSigned;
    hotbarVertices[hotbarVertexPosition + i * 5 + 1] = yCenter + yUnitSigned;
    hotbarVertices[hotbarVertexPosition + i * 5 + 2] = 0.0f;
    hotbarVertices[hotbarVertexPosition + i * 5 + 3] = textureCoords[i].x;
    hotbarVertices[hotbarVertexPosition + i * 5 + 4] = textureCoords[i].y;
  }

  hotbarIndices[position * 6 + 0] = 0 + position * 4;
  hotbarIndices[position * 6 + 1] = 1 + position * 4;
  hotbarIndices[position * 6 + 2] = 2 + position * 4;
  hotbarIndices[position * 6 + 3] = 2 + position * 4;
  hotbarIndices[position * 6 + 4] = 3 + position * 4;
  hotbarIndices[position * 6 + 5] = 0 + position * 4;
}

void UserInterface::DrawUIElement(Shader *shader, std::string texturePath, int elementWidth, int elementHeight, float centerX, float centerY)
{
  int width = Window::GetWidth();
  int height = Window::GetHeight();

  float xUnit = (float)elementWidth / width;
  float yUnit = (float)elementHeight / height;

  std::array<float, 20> vertices = {
      centerX - xUnit, centerY - yUnit, 0.0f, 0.0f, 0.0f,
      centerX + xUnit, centerY - yUnit, 0.0f, 1.0f, 0.0f,
      centerX + xUnit, centerY + yUnit, 0.0f, 1.0f, 1.0f,
      centerX - xUnit, centerY + yUnit, 0.0f, 0.0f, 1.0f};

  std::array<unsigned int, 6> indices = {
      0, 1, 2,
      2, 3, 0};

  VertexArray va;
  VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));
  VertexBufferLayout layout;
  layout.Push(LayoutType::LT_FLOAT, 3);
  layout.Push(LayoutType::LT_FLOAT, 2);
  va.AddBuffer(vb, layout);

  IndexBuffer ib(indices.data(), indices.size());

  Texture texture(texturePath, true);
  texture.Bind(1);
  shader->SetUniform1i("uTexture", 1);

  Renderer renderer;

  renderer.Draw(va, ib, shader);
}

void UserInterface::DrawHotbarIcons(Shader *shader)
{
  shader->SetUniform1i("uTexture", 0);

  VertexArray va;
  VertexBuffer vb(hotbarVertices.data(), 5 * 4 * UI_HOTBAR_SIZE * sizeof(float));
  VertexBufferLayout layout;
  layout.Push(LayoutType::LT_FLOAT, 3);
  layout.Push(LayoutType::LT_FLOAT, 2);
  va.AddBuffer(vb, layout);

  IndexBuffer ib(hotbarIndices.data(), 6 * UI_HOTBAR_SIZE);

  Renderer renderer;

  renderer.Draw(va, ib, shader);
}

void UserInterface::DrawUI(Shader *shader, Texture *atlas, int hotbarPosition)
{
  GLint polygonMode;
  glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float selectorCenterX = (-(int)floor(UI_HOTBAR_SIZE / 2) * hotbarSlotSize * 2 + hotbarPosition * hotbarSlotSize * 2) / Window::GetWidth();

  shader->Bind();

  DrawUIElement(shader, "extras/textures/crosshair.png", crosshairWidth, crosshairHeight, crosshairCenterX, crosshairCenterY);
  DrawUIElement(shader, "extras/textures/hotbar.png", hotbarWidth, hotbarHeight, hotbarCenterX, hotbarCenterY);
  DrawUIElement(shader, "extras/textures/hotbar_selector.png", hotbarSelectorWidth, hotbarSelectorHeight, selectorCenterX, hotbarCenterY);

  atlas->Bind(0);

  DrawHotbarIcons(shader);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
}