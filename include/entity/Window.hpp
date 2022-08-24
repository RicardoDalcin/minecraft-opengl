#ifndef _WINDOW_H
#define _WINDOW_H

#include <GLFW/glfw3.h>

class Window
{
private:
  Window() {}

  static float screenRatio;
  static float deltaTime;
  static float lastFrame;

  static GLFWwindow *window;

public:
  static bool Init();
  static void Tick();
  static void EndFrame();
  static void Terminate();

  static void ErrorCallback(int error, const char *description);
  static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

  static float GetScreenRatio();
  static float GetDeltaTime();
  static float GetLastFrame();

  static bool GetShouldClose();

  static GLFWwindow *GetWindow();
};

#endif