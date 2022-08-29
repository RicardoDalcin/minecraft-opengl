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

void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  if (key >= 0 && key < GLFW_KEY_LAST)
    Input::pressedKeys[key] = action != GLFW_RELEASE;

  for (auto &callback : keyCallbacks)
    callback(key, scancode, action, mods);
}

void Input::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    Input::pressedMouseButtons[button] = action != GLFW_RELEASE;

  for (auto &callback : mouseButtonCallbacks)
    callback(button, action, mods);
}

void Input::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  for (auto &callback : scrollCallbacks)
    callback(xoffset, yoffset);
}

void Input::resetDeltas()
{
  Input::deltaMousePosition = glm::vec2(0.0f, 0.0f);
}

void Input::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
  mousePosition.x = (float)xpos;
  mousePosition.y = (float)ypos;

  if (!initialized)
  {
    lastMousePosition = mousePosition;
    initialized = true;
  }

  // Accumulate delta in case of multiple polls per frame
  deltaMousePosition = mousePosition - lastMousePosition;
  lastMousePosition = mousePosition;
}

bool Input::isKeyPressed(int key)
{
  if (key >= 0 && key < GLFW_KEY_LAST)
    return Input::pressedKeys[key];

  return false;
}

bool Input::isMouseButtonPressed(int button)
{
  if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    return Input::pressedMouseButtons[button];

  return false;
}

glm::vec2 Input::getMousePosition()
{
  return mousePosition;
}

glm::vec2 Input::getDeltaMousePosition()
{
  return deltaMousePosition;
}

void Input::registerKeyCallback(KeyCallbackType callback)
{
  keyCallbacks.push_back(callback);
}

void Input::registerMouseButtonCallback(MouseButtonCallbackType callback)
{
  mouseButtonCallbacks.push_back(callback);
}

void Input::registerScrollCallback(ScrollCallbackType callback)
{
  scrollCallbacks.push_back(callback);
}