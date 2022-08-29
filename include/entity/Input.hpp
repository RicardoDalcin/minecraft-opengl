#ifndef _INPUT_H
#define _INPUT_H

#include <algorithm>

#include <glm/glm.hpp>

#include "core.h"

typedef std::function<void(int, int, int, int)> KeyCallbackType;
typedef std::function<void(int, int, int)> MouseButtonCallbackType;
typedef std::function<void(int, int)> ScrollCallbackType;

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

  static std::vector<KeyCallbackType> keyCallbacks;
  static std::vector<MouseButtonCallbackType> mouseButtonCallbacks;
  static std::vector<ScrollCallbackType> scrollCallbacks;

public:
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

  static bool isKeyPressed(int key);
  static bool isMouseButtonPressed(int button);

  static glm::vec2 getMousePosition();
  static glm::vec2 getDeltaMousePosition();

  static void resetDeltas();

  static void registerKeyCallback(KeyCallbackType callback);
  static void registerMouseButtonCallback(MouseButtonCallbackType callback);
  static void registerScrollCallback(ScrollCallbackType callback);
};

#endif