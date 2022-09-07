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
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
  static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

  static bool IsKeyPressed(int key);
  static bool IsMouseButtonPressed(int button);

  static glm::vec2 GetMousePosition();
  static glm::vec2 GetDeltaMousePosition();

  static void ResetDeltas();

  static void RegisterKeyCallback(KeyCallbackType callback);
  static void RegisterMouseButtonCallback(MouseButtonCallbackType callback);
  static void RegisterScrollCallback(ScrollCallbackType callback);
};

#endif