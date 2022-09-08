#include <cstdio>

#include "entity/Window.hpp"

#include "entity/Input.hpp"

float Window::screenRatio = 0.0f;
float Window::deltaTime = 0.0f;
float Window::lastFrame = 0.0f;

int Window::width = 0.0f;
int Window::height = 0.0f;

GLFWwindow *Window::window = nullptr;

bool Window::Init(bool fullscreen)
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

  int windowWidth;
  int windowHeight;

  if (fullscreen)
  {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    windowWidth = mode->width;
    windowHeight = mode->height;

    window = glfwCreateWindow(windowWidth, windowHeight, "Minecraft", glfwGetPrimaryMonitor(), NULL);
  }
  else
  {
    windowWidth = 1280;
    windowHeight = 720;

    window = glfwCreateWindow(windowWidth, windowHeight, "Minecraft", NULL, NULL);
  }

  width = windowWidth;
  height = windowHeight;

  screenRatio = (float)windowWidth / windowHeight;

  if (!window)
  {
    glfwTerminate();
    return false;
  }

  glfwSetKeyCallback(window, Input::KeyCallback);
  glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
  glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
  glfwSetScrollCallback(window, Input::ScrollCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowSize(window, width, height);

  glfwMakeContextCurrent(window);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *glversion = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  return true;
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

int Window::GetWidth()
{
  return width;
}

int Window::GetHeight()
{
  return height;
}

float Window::GetFPS()
{
  return 1.0f / deltaTime;
}