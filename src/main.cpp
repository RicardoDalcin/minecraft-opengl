#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <chrono>

#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

#include "core.h"

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

#include "world/BlockDatabase.hpp"
#include "world/World.hpp"

#include "entity/Character.hpp"

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
    Character player(glm::vec4(0.0f, 64.0f, -3.0f, 1.0f));

    Shader shader("src/Cube.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    BlockDatabase::Initialize();

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    World world(&shader);

    world.UpdateMeshes();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    printf("Elapsed time: %f \n", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    Input::registerMouseButtonCallback(std::bind(&Character::OnClick, &player, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    Input::registerScrollCallback(std::bind(&Character::OnScroll, &player, std::placeholders::_1, std::placeholders::_2));

    while (!Window::GetShouldClose())
    {
      Window::Tick();

      renderer.Clear();

      if (Input::isKeyPressed(GLFW_KEY_O))
      {
        camera.useOrthographic();
      }

      if (Input::isKeyPressed(GLFW_KEY_P))
      {
        camera.usePerspective();
      }

      if (Input::isKeyPressed(GLFW_KEY_V))
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      if (Input::isKeyPressed(GLFW_KEY_B))
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      player.Update(&camera, &world);

      world.Draw(&camera, camera.computeViewMatrix(), camera.computeProjectionMatrix());

      Window::EndFrame();
    }
  }

  Window::Terminate();

  return 0;
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
