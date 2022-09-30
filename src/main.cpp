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

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader/tiny_obj_loader.h>

#include "core/utils.hpp"
#include "core/Matrices.hpp"

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
#include "entity/UserInterface.hpp"

#include "world/BlockDatabase.hpp"
#include "world/World.hpp"
#include "world/Object.hpp"

#include "entity/Character.hpp"

int main()
{
  if (!Window::Init())
  {
    fprintf(stderr, "ERROR: Window initialization failed.\n");
    std::exit(EXIT_FAILURE);
  }

  {
    BlockDatabase::Initialize();

    Renderer renderer;

    Camera camera(-0.1f, -1024.0f, 60.0f);
    Character player(glm::vec4(0.0f, 64.0f, -3.0f, 1.0f));

    Shader worldShader("extras/shaders/World.shader");
    Shader interfaceShader("extras/shaders/Interface.shader");
    Shader objectShader("extras/shaders/Object.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    World world(&worldShader);

    world.UpdateMeshes();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    printf("Elapsed time: %f \n", (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

    Input::RegisterMouseButtonCallback(std::bind(&Character::OnClick, &player, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    Input::RegisterKeyCallback(std::bind(&Character::OnKeypress, &player, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    Input::RegisterScrollCallback(std::bind(&Character::OnScroll, &player, std::placeholders::_1, std::placeholders::_2));

    Object cow("extras/models/cow.obj", 0.0f, 0.0f);

    while (!Window::GetShouldClose())
    {
      Window::Tick();

      renderer.Clear();

      float fps = Window::GetFPS();

      printf("FPS: %f \n", fps);

      if (Input::IsKeyPressed(GLFW_KEY_O))
      {
        camera.UseOrthographic();
      }

      if (Input::IsKeyPressed(GLFW_KEY_P))
      {
        camera.UsePerspective();
      }

      if (Input::IsKeyPressed(GLFW_KEY_V))
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      if (Input::IsKeyPressed(GLFW_KEY_B))
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      player.Update(&camera, &world);

      world.Draw(&camera, camera.ComputeViewMatrix(), camera.ComputeProjectionMatrix());

      cow.Draw(&objectShader, camera.ComputeViewMatrix(), camera.ComputeProjectionMatrix());

      UserInterface::DrawUI(&interfaceShader, world.GetTextureAtlas(), player.GetHotbarPosition());

      Window::EndFrame();
    }
  }

  Window::Terminate();

  return 0;
}
