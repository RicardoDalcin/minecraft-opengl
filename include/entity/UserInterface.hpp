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

#include "entity/Character.hpp"

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

public:
  static void DrawUIElement(std::string texturePath, int elementWidth, int elementHeight, float centerX, float centerY);

  static void DrawUI(int hotbarPosition);
};

#endif