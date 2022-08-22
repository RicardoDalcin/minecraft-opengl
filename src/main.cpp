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

#include "world/Cubes.hpp"
#include "world/Chunk.hpp"

GLuint BuildTriangles();                                                     // Constrói triângulos para renderização
GLuint LoadShader_Vertex(const char *filename);                              // Carrega um vertex shader
GLuint LoadShader_Fragment(const char *filename);                            // Carrega um fragment shader
void LoadShader(const char *filename, GLuint shader_id);                     // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

struct SceneObject
{
  const char *name;     // Nome do objeto
  void *firstIndex;     // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
  int indicesNumber;    // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
  GLenum renderingMode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

std::map<const char *, SceneObject> g_VirtualScene;

struct PressedKeys
{
  bool front;
  bool back;
  bool left;
  bool right;
};

PressedKeys g_PressedKeys = {false, false, false, false};

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 16.0f / 9.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 3.141592f / 4; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 3.141592f / 6;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 2.5f;       // Distância da câmera para a origem

glm::vec4 g_CameraCenter = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f);
glm::vec4 g_CameraTarget = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);

const glm::vec4 g_CameraUp = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

float g_DeltaTime = 0.0f;
float g_LastFrame = 0.0f;

// Variável que controla o tipo de projeção utilizada: perspectiva ou ortográfica.
bool g_UsePerspectiveProjection = true;

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

  glfwSetKeyCallback(window, KeyCallback);

  glfwSetMouseButtonCallback(window, MouseButtonCallback);

  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  glfwSetScrollCallback(window, ScrollCallback);

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

    Shader shader("src/Cube.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Chunk defaultChunk(&shader);

    while (!glfwWindowShouldClose(window))
    {
      g_DeltaTime = glfwGetTime() - g_LastFrame;
      g_LastFrame = glfwGetTime();

      renderer.Clear();

      float x = cos(g_CameraPhi) * sin(g_CameraTheta);
      float y = sin(g_CameraPhi);
      float z = cos(g_CameraPhi) * cos(g_CameraTheta);

      glm::vec4 cameraFront = glm::normalize(glm::vec4(x, y, z, 0.0f));

      float cameraSpeed = 2.5f * g_DeltaTime;

      if (g_PressedKeys.front)
      {
        g_CameraCenter += cameraSpeed * cameraFront;
      }

      if (g_PressedKeys.back)
      {
        g_CameraCenter -= cameraSpeed * cameraFront;
      }

      if (g_PressedKeys.left)
      {
        glm::vec4 w = -cameraFront / norm(cameraFront);
        glm::vec4 u = crossproduct(g_CameraUp, w) / norm(crossproduct(g_CameraUp, w));

        g_CameraCenter -= cameraSpeed * u;
      }

      if (g_PressedKeys.right)
      {
        glm::vec4 w = -cameraFront / norm(cameraFront);
        glm::vec4 u = crossproduct(g_CameraUp, w) / norm(crossproduct(g_CameraUp, w));

        g_CameraCenter += cameraSpeed * u;
      }

      glm::mat4 projection;

      float nearplane = -0.1f;
      float farplane = -50.0f;

      if (g_UsePerspectiveProjection)
      {
        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
      }
      else
      {
        float t = 1.5f * g_CameraDistance / 2.5f;
        float b = -t;
        float r = t * g_ScreenRatio;
        float l = -r;
        projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
      }

      glm::mat4 view = Matrix_Camera_View(g_CameraCenter, cameraFront, g_CameraUp);

      defaultChunk.Draw(view, projection);

      glfwSwapBuffers(window);
      glfwPollEvents();
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

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
    // posição atual do cursor nas variáveis g_LastCursorPosX e
    // g_LastCursorPosY.  Também, setamos a variável
    // g_LeftMouseButtonPressed como true, para saber que o usuário está
    // com o botão esquerdo pressionado.
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
    g_LeftMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
  {
    // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
    // variável abaixo para false.
    g_LeftMouseButtonPressed = false;
  }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
  // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
  // pressionado, computamos quanto que o mouse se movimento desde o último
  // instante de tempo, e usamos esta movimentação para atualizar os
  // parâmetros que definem a posição da câmera dentro da cena virtual.
  // Assim, temos que o usuário consegue controlar a câmera.

  if (!g_LeftMouseButtonPressed)
    return;

  // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
  float dx = xpos - g_LastCursorPosX;
  float dy = ypos - g_LastCursorPosY;

  // Atualizamos parâmetros da câmera com os deslocamentos
  g_CameraTheta -= 0.003f * dx;
  g_CameraPhi -= 0.003f * dy;

  // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
  float phimax = 3.141592f / 2;
  float phimin = -phimax;

  if (g_CameraPhi > phimax)
    g_CameraPhi = phimax;

  if (g_CameraPhi < phimin)
    g_CameraPhi = phimin;

  // Atualizamos as variáveis globais para armazenar a posição atual do
  // cursor como sendo a última posição conhecida do cursor.
  g_LastCursorPosX = xpos;
  g_LastCursorPosY = ypos;
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
  // Atualizamos a distância da câmera para a origem utilizando a
  // movimentação da "rodinha", simulando um ZOOM.
  g_CameraDistance -= 0.1f * yoffset;

  // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
  // onde ela está olhando, pois isto gera problemas de divisão por zero na
  // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
  // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
  // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
  const float verysmallnumber = std::numeric_limits<float>::epsilon();
  if (g_CameraDistance < verysmallnumber)
    g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mod)
{
  // ================
  // Não modifique este loop! Ele é utilizando para correção automatizada dos
  // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
  for (int i = 0; i < 10; ++i)
    if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
      std::exit(100 + i);
  // ================

  // Se o usuário pressionar a tecla ESC, fechamos a janela.
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  // O código abaixo implementa a seguinte lógica:
  //   Se apertar tecla X       então g_AngleX += delta;
  //   Se apertar tecla shift+X então g_AngleX -= delta;
  //   Se apertar tecla Y       então g_AngleY += delta;
  //   Se apertar tecla shift+Y então g_AngleY -= delta;
  //   Se apertar tecla Z       então g_AngleZ += delta;
  //   Se apertar tecla shift+Z então g_AngleZ -= delta;

  float delta = 3.141592 / 16; // 22.5 graus, em radianos.

  if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  }

  if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  }
  if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  }

  // Se o usuário apertar a tecla espaço, resetamos os ângulos de Euler para zero.
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
  {
    g_AngleX = 0.0f;
    g_AngleY = 0.0f;
    g_AngleZ = 0.0f;
  }

  // Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
  if (key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    g_UsePerspectiveProjection = true;
  }

  // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
  if (key == GLFW_KEY_O && action == GLFW_PRESS)
  {
    g_UsePerspectiveProjection = false;
  }

  if (key == GLFW_KEY_W)
  {
    if (action == GLFW_PRESS)
      g_PressedKeys.front = true;

    if (action == GLFW_RELEASE)
      g_PressedKeys.front = false;
  }

  if (key == GLFW_KEY_S)
  {
    if (action == GLFW_PRESS)
      g_PressedKeys.back = true;

    if (action == GLFW_RELEASE)
      g_PressedKeys.back = false;
  }

  if (key == GLFW_KEY_A)
  {
    if (action == GLFW_PRESS)
      g_PressedKeys.left = true;

    if (action == GLFW_RELEASE)
      g_PressedKeys.left = false;
  }

  if (key == GLFW_KEY_D)
  {
    if (action == GLFW_PRESS)
      g_PressedKeys.right = true;

    if (action == GLFW_RELEASE)
      g_PressedKeys.right = false;
  }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char *description)
{
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
