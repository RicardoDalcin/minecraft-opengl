#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include "core.h"

#include "engine/Texture.hpp"
#include "engine/Shader.hpp"
#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/VertexBufferLayout.hpp"
#include "engine/IndexBuffer.hpp"
#include "engine/Renderer.hpp"

#include "entity/Window.hpp"

const int UI_HOTBAR_SIZE = 9;

// Classe para gerenciamento e renderização da interface de usuário
class UserInterface
{
private:
  UserInterface() {}

  static const float crosshairWidth;
  static const float crosshairHeight;

  static const float crosshairCenterX;
  static const float crosshairCenterY;

  static const float hotbarSlotSize;

  static const float hotbarWidth;
  static const float hotbarHeight;

  static const float hotbarCenterX;
  static const float hotbarCenterY;

  static const float hotbarSelectorWidth;
  static const float hotbarSelectorHeight;

  static const float hotbarIconWidth;
  static const float hotbarIconHeight;

  static std::array<float, 5 * 4 * UI_HOTBAR_SIZE> hotbarVertices;
  static std::array<unsigned int, 6 * UI_HOTBAR_SIZE> hotbarIndices;

public:
  static void UpdateHotbarPosition(int position, std::array<glm::vec2, 4> textureCoords);

  static void DrawUIElement(Shader *shader, std::string texturePath, int elementWidth, int elementHeight, float centerX, float centerY);

  static void DrawHotbarIcons(Shader *shader);

  static void DrawUI(Shader *shader, Texture *atlas, int hotbarPosition);
};

#endif