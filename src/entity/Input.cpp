#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

#include "entity/Input.hpp"

bool Input::pressedKeys[GLFW_KEY_LAST] = {};
bool Input::pressedMouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};

glm::vec2 Input::mousePosition = glm::vec2(0.0f, 0.0f);
glm::vec2 Input::deltaMousePosition = glm::vec2(0.0f, 0.0f);

glm::vec2 Input::lastMousePosition = glm::vec2(0.0f, 0.0f);
bool Input::initialized = false;

std::vector<KeyCallbackType> Input::keyCallbacks = {};
std::vector<MouseButtonCallbackType> Input::mouseButtonCallbacks = {};
std::vector<ScrollCallbackType> Input::scrollCallbacks = {};

// Gerencia o key callback do GLFW
void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  // Atualiza o estado da tecla
  if (key >= 0 && key < GLFW_KEY_LAST)
    Input::pressedKeys[key] = action != GLFW_RELEASE;

  // Executa os callbacks
  for (auto &callback : keyCallbacks)
    callback(key, scancode, action, mods);
}

// Gerencia o mouse button callback do GLFW
void Input::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  // Atualiza o estado do botão
  if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    Input::pressedMouseButtons[button] = action != GLFW_RELEASE;

  // Executa os callbacks
  for (auto &callback : mouseButtonCallbacks)
    callback(button, action, mods);
}

// Gerencia o scroll callback do GLFW
void Input::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  // Executa os callbacks
  for (auto &callback : scrollCallbacks)
    callback(xoffset, yoffset);
}

void Input::ResetDeltas()
{
  Input::deltaMousePosition = glm::vec2(0.0f, 0.0f);
}

// Gerencia o cursor position callback do GLFW
void Input::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
  mousePosition.x = (float)xpos;
  mousePosition.y = (float)ypos;

  if (!initialized)
  {
    lastMousePosition = mousePosition;
    initialized = true;
  }

  // Acumula o delta caso tiver múltiplos polls por frame
  deltaMousePosition = mousePosition - lastMousePosition;
  lastMousePosition = mousePosition;
}

bool Input::IsKeyPressed(int key)
{
  if (key >= 0 && key < GLFW_KEY_LAST)
    return Input::pressedKeys[key];

  return false;
}

bool Input::IsMouseButtonPressed(int button)
{
  if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    return Input::pressedMouseButtons[button];

  return false;
}

glm::vec2 Input::GetMousePosition()
{
  return mousePosition;
}

glm::vec2 Input::GetDeltaMousePosition()
{
  return deltaMousePosition;
}

void Input::RegisterKeyCallback(KeyCallbackType callback)
{
  keyCallbacks.push_back(callback);
}

void Input::RegisterMouseButtonCallback(MouseButtonCallbackType callback)
{
  mouseButtonCallbacks.push_back(callback);
}

void Input::RegisterScrollCallback(ScrollCallbackType callback)
{
  scrollCallbacks.push_back(callback);
}