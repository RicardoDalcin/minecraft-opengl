#include <cstdio>

#include "entity/Window.hpp"

#include "entity/Input.hpp"

float Window::screenRatio = 0.0f;
float Window::deltaTime = 0.0f;
float Window::lastFrame = 0.0f;

GLFWwindow *Window::window = nullptr;

bool Window::Init()
{
  if (!glfwInit())
    return false;

  deltaTime = 0.0f;
  lastFrame = 0.0f;

  glfwSetErrorCallback(ErrorCallback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  int width = mode->width;
  int height = mode->height;

  // int width = 1280;
  // int height = 720;

  screenRatio = (float)width / height;

  window = glfwCreateWindow(width, height, "Minecraft", glfwGetPrimaryMonitor(), NULL);
  // window = glfwCreateWindow(width, height, "Minecraft", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return false;
  }

  glfwSetKeyCallback(window, Input::keyCallback);
  glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
  glfwSetCursorPosCallback(window, Input::cursorPositionCallback);
  glfwSetScrollCallback(window, Input::scrollCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowSize(window, width, height);

  glfwMakeContextCurrent(window);
}

void Window::Tick()
{
  glfwPollEvents();

  deltaTime = glfwGetTime() - lastFrame;
  lastFrame = glfwGetTime();
}

void Window::EndFrame()
{
  glfwSwapBuffers(window);
}

void Window::Terminate()
{
  glfwTerminate();
}

void Window::ErrorCallback(int error, const char *description)
{
  fprintf(stderr, "ERROR: %s\n", description);
}

void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
  screenRatio = (float)width / height;
}

float Window::GetScreenRatio()
{
  return screenRatio;
}

float Window::GetDeltaTime()
{
  return deltaTime;
}

float Window::GetLastFrame()
{
  return lastFrame;
}

bool Window::GetShouldClose()
{
  return glfwWindowShouldClose(window);
}

GLFWwindow *Window::GetWindow()
{
  return window;
}
