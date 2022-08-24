#include <cmath>
#include <cstdio>
#include <cstdlib>

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

#include "world/Chunk.hpp"

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ErrorCallback(int error, const char *description);

float g_ScreenRatio = 16.0f / 9.0f;

float g_DeltaTime = 0.0f;
float g_LastFrame = 0.0f;

int main()
{
  int success = glfwInit();
  if (!success)
  {
    fprintf(stderr, "ERROR: glfwInit() failed.\n");
    std::exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(ErrorCallback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window;
  window = glfwCreateWindow(1280, 720, "Minecraft", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
    std::exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, Input::keyCallback);
  glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
  glfwSetCursorPosCallback(window, Input::cursorPositionCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowSize(window, 1280, 720); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

  glfwMakeContextCurrent(window);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *glversion = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  {
    Renderer renderer;

    Camera camera(-0.1f, -50.0f, 3.141592 / 3.0f);
    Shader shader("src/Cube.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Chunk defaultChunk(&shader);

    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();

      g_DeltaTime = glfwGetTime() - g_LastFrame;
      g_LastFrame = glfwGetTime();

      renderer.Clear();

      if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
      {
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
      }

      float baseSpeed = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT) ? 10.0f : 2.5f;

      float cameraSpeed = baseSpeed * g_DeltaTime;

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

      defaultChunk.Draw(camera.computeViewMatrix(), camera.computeProjectionMatrix());

      glfwSwapBuffers(window);
    }
  }

  glfwTerminate();

  return 0;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
  // Indicamos que queremos renderizar em toda região do framebuffer. A
  // função "glViewport" define o mapeamento das "normalized device
  // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
  // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
  glViewport(0, 0, width, height);

  // Atualizamos também a razão que define a proporção da janela (largura /
  // altura), a qual será utilizada na definição das matrizes de projeção,
  // tal que não ocorra distorções durante o processo de "Screen Mapping"
  // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
  //
  // O cast para float é necessário pois números inteiros são arredondados ao
  // serem divididos!
  g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char *description)
{
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
