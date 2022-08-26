#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <chrono>

#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image/stb_image.h>

#include "core/utils.hpp"
#include "core/matrices.hpp"

#include "engine/IndexBuffer.hpp"
#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/VertexBufferLayout.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"
#include "engine/Renderer.hpp"

#include "entity/Camera.hpp"
#include "entity/Input.hpp"
#include "entity/Window.hpp"

// #include "world/Chunk.hpp"
#include "world/BlockDatabase.hpp"
#include "world/World.hpp"

int main()
{
  if (!Window::Init())
  {
    fprintf(stderr, "ERROR: Window initialization failed.\n");
    std::exit(EXIT_FAILURE);
  }

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *glversion = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  {
    Renderer renderer;

    Camera camera(-0.1f, -1024.0f, 3.141592 / 3.0f);
    Shader shader("src/Cube.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Chunk defaultChunk(&shader);

    BlockDatabase::Initialize();

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    World world(&shader);

    world.UpdateMeshes();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    printf("Elapsed time: %f \n", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    while (!Window::GetShouldClose())
    {
      Window::Tick();

      renderer.Clear();

      // if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
      // {
      glm::vec2 deltaPos = Input::getDeltaMousePosition();

      float newTheta = camera.getCameraTheta() - 0.003f * deltaPos.x;
      float newPhi = camera.getCameraPhi() - 0.003f * deltaPos.y;

      // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
      float phimax = 3.141592f / 2;
      float phimin = -phimax;

      if (newPhi > phimax)
        newPhi = phimax;

      if (newPhi < phimin)
        newPhi = phimin;

      camera.updateCameraAngles(newTheta, newPhi);
      Input::resetDeltas();
      // }

      float baseSpeed = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 10.0f : 2.5f;

      float cameraSpeed = baseSpeed * Window::GetDeltaTime();

      if (Input::isKeyPressed(GLFW_KEY_O))
      {
        camera.useOrthographic();
      }

      if (Input::isKeyPressed(GLFW_KEY_P))
      {
        camera.usePerspective();
      }

      glm::vec4 newCameraPosition = camera.getPosition();
      if (Input::isKeyPressed(GLFW_KEY_W))
      {
        newCameraPosition += camera.getTarget() * cameraSpeed;
      }

      if (Input::isKeyPressed(GLFW_KEY_S))
      {
        newCameraPosition -= camera.getTarget() * cameraSpeed;
      }

      if (Input::isKeyPressed(GLFW_KEY_A))
      {
        newCameraPosition -= camera.getRight() * cameraSpeed;
      }

      if (Input::isKeyPressed(GLFW_KEY_D))
      {
        newCameraPosition += camera.getRight() * cameraSpeed;
      }

      camera.updatePosition(newCameraPosition);

      // defaultChunk.Draw(camera.computeViewMatrix(), camera.computeProjectionMatrix());
      world.Draw(camera.computeViewMatrix(), camera.computeProjectionMatrix());

      Window::EndFrame();
    }
  }

  Window::Terminate();

  return 0;
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
