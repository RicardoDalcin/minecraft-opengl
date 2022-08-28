#ifndef _INPUT_H
#define _INPUT_H

#include <glm/glm.hpp>

#include "core.h"

class Input
{
private:
  Input() {}

  static bool pressedKeys[GLFW_KEY_LAST];
  static bool pressedMouseButtons[GLFW_MOUSE_BUTTON_LAST];

  static glm::vec2 mousePosition;
  static glm::vec2 deltaMousePosition;

  static glm::vec2 lastMousePosition;
  static bool initialized;

public:
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);

  static bool isKeyPressed(int key);
  static bool isMouseButtonPressed(int button);

  static glm::vec2 getMousePosition();
  static glm::vec2 getDeltaMousePosition();

  static void resetDeltas();
};

#endif