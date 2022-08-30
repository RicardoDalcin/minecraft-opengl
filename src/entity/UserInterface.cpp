#include "entity/UserInterface.hpp"

const float UserInterface::crosshairWidth = 32.0f;
const float UserInterface::crosshairHeight = 32.0f;

const float UserInterface::crosshairCenterX = 0.0f;
const float UserInterface::crosshairCenterY = 0.0f;

const float UserInterface::hotbarSlotSize = 64.0f;

const float UserInterface::hotbarWidth = hotbarSlotSize * 9;
const float UserInterface::hotbarHeight = hotbarSlotSize;

const float UserInterface::hotbarCenterX = 0.0f;
const float UserInterface::hotbarCenterY = -0.9f;

const float UserInterface::hotbarSelectorWidth = 68.0f;
const float UserInterface::hotbarSelectorHeight = 68.0f;

void UserInterface::DrawUIElement(std::string texturePath, int elementWidth, int elementHeight, float centerX, float centerY)
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

  Shader shader("src/UI.shader");
  shader.Bind();

  Texture texture(texturePath, true);
  texture.Bind(1);
  shader.SetUniform1i("uTexture", 1);

  Renderer renderer;

  renderer.Draw(va, ib, shader);
}

void UserInterface::DrawUI(int hotbarPosition)
{
  GLint polygonMode;
  glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float selectorCenterX = (-(int)floor(HOTBAR_SIZE / 2) * hotbarSlotSize * 2 + hotbarPosition * hotbarSlotSize * 2) / Window::GetWidth();

  DrawUIElement("extras/textures/crosshair.png", crosshairWidth, crosshairHeight, crosshairCenterX, crosshairCenterY);
  DrawUIElement("extras/textures/hotbar.png", hotbarWidth, hotbarHeight, hotbarCenterX, hotbarCenterY);
  DrawUIElement("extras/textures/hotbar_selector.png", hotbarSelectorWidth, hotbarSelectorHeight, selectorCenterX, hotbarCenterY);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
}