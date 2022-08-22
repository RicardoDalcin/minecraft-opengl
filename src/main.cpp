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

  // GLuint vertex_shader_id = LoadShader_Vertex("src/shader_vertex.glsl");
  // GLuint fragment_shader_id = LoadShader_Fragment("src/shader_fragment.glsl");

  // GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

  // GLuint vertex_array_object_id = BuildTriangles();

  // GLint model_uniform = glGetUniformLocation(program_id, "model");
  // GLint view_uniform = glGetUniformLocation(program_id, "view");
  // GLint projection_uniform = glGetUniformLocation(program_id, "projection");
  // GLint render_as_black_uniform = glGetUniformLocation(program_id, "render_as_black");

  // glEnable(GL_DEPTH_TEST);

  {
    int x = 2;
    int y = 0;

    float spriteWidth = 16.0f;
    float spriteHeight = 16.0f;
    float sheetWidth = 256.0f;
    float sheetHeight = 256.0f;

    // float positions[] = {
    //     0.0f, 0.0f, (x * spriteWidth) / sheetHeight, (y * spriteHeight) / sheetHeight,
    //     400.0f, 0.0f, ((x + 1) * spriteWidth) / sheetWidth, (y * spriteHeight) / sheetHeight,
    //     400.0f, 400.0f, ((x + 1) * spriteWidth) / sheetWidth, ((y + 1) * spriteHeight) / sheetHeight,
    //     0.0f, 400.0f, (x * spriteWidth) / sheetHeight, ((y + 1) * spriteHeight) / sheetHeight};

    // float positions[] = {
    //     0.0f, 0.0f, 0.0f, 0.0f,
    //     400.0f, 0.0f, 1.0f, 0.0f,
    //     400.0f, 400.0f, 1.0f, 1.0f,
    //     0.0f, 400.0f, 0.0f, 1.0f};

    // unsigned int indices[] = {
    //     0, 1, 2,
    //     2, 3, 0};

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // unsigned int vao;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);

    // VertexArray va;
    // VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    // VertexBufferLayout layout;
    // layout.Push(LayoutType::LT_FLOAT, 2);
    // layout.Push(LayoutType::LT_FLOAT, 2);
    // va.AddBuffer(vb, layout);

    // IndexBuffer ib(indices, 6);

    // glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    // glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    // Shader shader("src/Basic.shader");
    // shader.Bind();
    // shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    // Texture texture("extras/textures/BlockAtlas.png");
    // texture.Bind();
    // shader.SetUniform1i("u_Texture", 0);

    // va.Unbind();
    // vb.Unbind();
    // ib.Unbind();
    // shader.Unbind();

    // Renderer renderer;

    // glm::vec3 translationA(0, 0, 0);
    // glm::vec3 translationB(400, 200, 0);

    // float r = 0.0f;
    // float increment = 0.01f;

    Renderer renderer;

    Shader shader("src/Cube.shader");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // MinecraftClone::Cubes::init(shader);

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

      // MinecraftClone::Cubes::update(g_DeltaTime, shader, view, projection);
      defaultChunk.Draw(view, projection);

      // {
      //   glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
      //   glm::mat4 mvp = proj * view * model;
      //   shader.Bind();
      //   shader.SetUniformMat4f("u_MVP", mvp);

      //   renderer.Draw(va, ib, shader);
      // }

      // if (r > 1.0f)
      //   increment = -0.01f;
      // else if (r < 0.0f)
      //   increment = 0.01f;

      // r += increment;

      glfwSwapBuffers(window);
      glfwPollEvents();
      // g_DeltaTime = glfwGetTime() - g_LastFrame;
      // g_LastFrame = glfwGetTime();

      // float x = cos(g_CameraPhi) * sin(g_CameraTheta);
      // float y = sin(g_CameraPhi);
      // float z = cos(g_CameraPhi) * cos(g_CameraTheta);

      // glm::vec4 cameraFront = glm::normalize(glm::vec4(x, y, z, 0.0f));

      // float cameraSpeed = 2.5f * g_DeltaTime;

      // if (g_PressedKeys.front)
      // {
      //   g_CameraCenter += cameraSpeed * cameraFront;
      // }

      // if (g_PressedKeys.back)
      // {
      //   g_CameraCenter -= cameraSpeed * cameraFront;
      // }

      // if (g_PressedKeys.left)
      // {
      //   glm::vec4 w = -cameraFront / norm(cameraFront);
      //   glm::vec4 u = crossproduct(g_CameraUp, w) / norm(crossproduct(g_CameraUp, w));

      //   g_CameraCenter -= cameraSpeed * u;
      // }

      // if (g_PressedKeys.right)
      // {
      //   glm::vec4 w = -cameraFront / norm(cameraFront);
      //   glm::vec4 u = crossproduct(g_CameraUp, w) / norm(crossproduct(g_CameraUp, w));

      //   g_CameraCenter += cameraSpeed * u;
      // }

      // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // glUseProgram(program_id);

      // glBindVertexArray(vertex_array_object_id);

      // glm::mat4 projection;

      // float nearplane = -0.1f;
      // float farplane = -10.0f;

      // if (g_UsePerspectiveProjection)
      // {
      //   float field_of_view = 3.141592 / 3.0f;
      //   projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
      // }
      // else
      // {
      //   float t = 1.5f * g_CameraDistance / 2.5f;
      //   float b = -t;
      //   float r = t * g_ScreenRatio;
      //   float l = -r;
      //   projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
      // }

      // glm::mat4 view = Matrix_Camera_View(g_CameraCenter, cameraFront, g_CameraUp);

      // glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
      // glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

      // for (int i = 1; i <= 1; ++i)
      // {
      //   glm::mat4 model;

      //   if (i == 1)
      //   {
      //     model = Matrix_Identity();
      //   }

      //   glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

      //   glUniform1i(render_as_black_uniform, false);

      //   glDrawElements(
      //       g_VirtualScene["cube_faces"].renderingMode,
      //       g_VirtualScene["cube_faces"].indicesNumber,
      //       GL_UNSIGNED_INT,
      //       (void *)g_VirtualScene["cube_faces"].firstIndex);

      //   glLineWidth(4.0f);

      //   glDrawElements(
      //       g_VirtualScene["axes"].renderingMode,
      //       g_VirtualScene["axes"].indicesNumber,
      //       GL_UNSIGNED_INT,
      //       (void *)g_VirtualScene["axes"].firstIndex);

      //   glUniform1i(render_as_black_uniform, true);

      //   glDrawElements(
      //       g_VirtualScene["cube_edges"].renderingMode,
      //       g_VirtualScene["cube_edges"].indicesNumber,
      //       GL_UNSIGNED_INT,
      //       (void *)g_VirtualScene["cube_edges"].firstIndex);

      //   if (i == 3)
      //   {
      //     glPointSize(15.0f);
      //     glDrawArrays(GL_POINTS, 3, 1);
      //   }
      // }

      // glm::mat4 model = Matrix_Identity();

      // glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

      // glLineWidth(10.0f);

      // glUniform1i(render_as_black_uniform, false);

      // glDrawElements(
      //     g_VirtualScene["axes"].renderingMode,
      //     g_VirtualScene["axes"].indicesNumber,
      //     GL_UNSIGNED_INT,
      //     (void *)g_VirtualScene["axes"].firstIndex);

      // glBindVertexArray(0);

      // glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);

      // glfwSwapBuffers(window);

      // glfwPollEvents();
    }
  }

  glfwTerminate();

  return 0;
}

// Constrói triângulos para futura renderização
GLuint BuildTriangles()
{
  // Primeiro, definimos os atributos de cada vértice.

  // A posição de cada vértice é definida por coeficientes em um sistema de
  // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
  // homogêneas.  Veja as seguintes referências:
  //
  //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
  //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
  //
  // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
  //
  GLfloat model_coefficients[] = {
      // Vértices de um cubo
      //    X      Y     Z     W
      -0.5f, 0.5f, 0.5f, 1.0f,   // posição do vértice 0
      -0.5f, -0.5f, 0.5f, 1.0f,  // posição do vértice 1
      0.5f, -0.5f, 0.5f, 1.0f,   // posição do vértice 2
      0.5f, 0.5f, 0.5f, 1.0f,    // posição do vértice 3
      -0.5f, 0.5f, -0.5f, 1.0f,  // posição do vértice 4
      -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 5
      0.5f, -0.5f, -0.5f, 1.0f,  // posição do vértice 6
      0.5f, 0.5f, -0.5f, 1.0f,   // posição do vértice 7
                                 // Vértices para desenhar o eixo X
                                 //    X      Y     Z     W
      0.0f, 0.0f, 0.0f, 1.0f,    // posição do vértice 8
      1.0f, 0.0f, 0.0f, 1.0f,    // posição do vértice 9
                                 // Vértices para desenhar o eixo Y
                                 //    X      Y     Z     W
      0.0f, 0.0f, 0.0f, 1.0f,    // posição do vértice 10
      0.0f, 1.0f, 0.0f, 1.0f,    // posição do vértice 11
                                 // Vértices para desenhar o eixo Z
                                 //    X      Y     Z     W
      0.0f, 0.0f, 0.0f, 1.0f,    // posição do vértice 12
      0.0f, 0.0f, 1.0f, 1.0f,    // posição do vértice 13
  };

  // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
  // um buffer de memória que irá conter os valores de um certo atributo de
  // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
  // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
  // Agora criamos um VBO para armazenarmos um atributo: posição.
  GLuint VBO_model_coefficients_id;
  glGenBuffers(1, &VBO_model_coefficients_id);

  // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
  // contém a definição de vários atributos de um certo conjunto de vértices;
  // isto é, um VAO irá conter ponteiros para vários VBOs.
  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);

  // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
  // está contido na variável "vertex_array_object_id".
  glBindVertexArray(vertex_array_object_id);

  // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
  // variável VBO_model_coefficients_id será modificado a seguir. A
  // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
  // irá conter atributos de vértices.
  glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

  // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
  // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
  // para alocar um número de bytes exatamente igual ao tamanho ("size")
  // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
  // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
  // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
  // também dizemos que tais dados serão utilizados para renderizar ou
  // desenhar ("DRAW").  Pense que:
  //
  //            glBufferData()  ==  malloc() do C  ==  new do C++.
  //
  glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

  // Finalmente, copiamos os valores do array model_coefficients para dentro do
  // VBO "ligado" acima.  Pense que:
  //
  //            glBufferSubData()  ==  memcpy() do C.
  //
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

  // Precisamos então informar um índice de "local" ("location"), o qual será
  // utilizado no shader "shader_vertex.glsl" para acessar os valores
  // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
  // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
  // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
  // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
  // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
  // flutuante com 32 bits (GL_FLOAT).
  // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
  // está dentro do VAO "ligado" acima por glBindVertexArray().
  // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
  GLuint location = 0;            // "(location = 0)" em "shader_vertex.glsl"
  GLint number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

  // "Ativamos" os atributos. Informamos que os atributos com índice de local
  // definido acima, na variável "location", deve ser utilizado durante o
  // rendering.
  glEnableVertexAttribArray(location);

  // "Desligamos" o VBO, evitando assim que operações posteriores venham a
  // alterar o mesmo. Isso evita bugs.
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Agora repetimos todos os passos acima para atribuir um novo atributo a
  // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
  // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
  // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
  // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
  GLfloat color_coefficients[] = {
      // Cores dos vértices do cubo
      //  R     G     B     A
      1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
      1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
      0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
      0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
      1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 4
      1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 5
      0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 6
      0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 7
                              // Cores para desenhar o eixo X
      1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 8
      1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 9
                              // Cores para desenhar o eixo Y
      0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 10
      0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 11
                              // Cores para desenhar o eixo Z
      0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 12
      0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 13
  };
  GLuint VBO_color_coefficients_id;
  glGenBuffers(1, &VBO_color_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
  location = 1;             // "(location = 1)" em "shader_vertex.glsl"
  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Vamos então definir polígonos utilizando os vértices do array
  // model_coefficients.
  //
  // Para referência sobre os modos de renderização, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
  //
  // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
  //
  GLuint indices[] = {
      // Definimos os índices dos vértices que definem as FACES de um cubo
      // através de 12 triângulos que serão desenhados com o modo de renderização
      // GL_TRIANGLES.
      0, 1, 2, // triângulo 1
      7, 6, 5, // triângulo 2
      3, 2, 6, // triângulo 3
      4, 0, 3, // triângulo 4
      4, 5, 1, // triângulo 5
      1, 5, 6, // triângulo 6
      0, 2, 3, // triângulo 7
      7, 5, 4, // triângulo 8
      3, 6, 7, // triângulo 9
      4, 3, 7, // triângulo 10
      4, 1, 0, // triângulo 11
      1, 6, 2, // triângulo 12
               // Definimos os índices dos vértices que definem as ARESTAS de um cubo
               // através de 12 linhas que serão desenhadas com o modo de renderização
               // GL_LINES.
      0, 1,    // linha 1
      1, 2,    // linha 2
      2, 3,    // linha 3
      3, 0,    // linha 4
      0, 4,    // linha 5
      4, 7,    // linha 6
      7, 6,    // linha 7
      6, 2,    // linha 8
      6, 5,    // linha 9
      5, 4,    // linha 10
      5, 1,    // linha 11
      7, 3,    // linha 12
               // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
               // Z, que serão desenhados com o modo GL_LINES.
      8, 9,    // linha 1
      10, 11,  // linha 2
      12, 13   // linha 3
  };

  // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
  // coloridas do cubo.
  SceneObject cube_faces;
  cube_faces.name = "Cubo (faces coloridas)";
  cube_faces.firstIndex = (void *)0;       // Primeiro índice está em indices[0]
  cube_faces.indicesNumber = 36;           // Último índice está em indices[35]; total de 36 índices.
  cube_faces.renderingMode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

  // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
  g_VirtualScene["cube_faces"] = cube_faces;

  // Criamos um segundo objeto virtual (SceneObject) que se refere às arestas
  // pretas do cubo.
  SceneObject cube_edges;
  cube_edges.name = "Cubo (arestas pretas)";
  cube_edges.firstIndex = (void *)(36 * sizeof(GLuint)); // Primeiro índice está em indices[36]
  cube_edges.indicesNumber = 24;                         // Último índice está em indices[59]; total de 24 índices.
  cube_edges.renderingMode = GL_LINES;                   // Índices correspondem ao tipo de rasterização GL_LINES.

  // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
  g_VirtualScene["cube_edges"] = cube_edges;

  // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
  SceneObject axes;
  axes.name = "Eixos XYZ";
  axes.firstIndex = (void *)(60 * sizeof(GLuint)); // Primeiro índice está em indices[60]
  axes.indicesNumber = 6;                          // Último índice está em indices[65]; total de 6 índices.
  axes.renderingMode = GL_LINES;                   // Índices correspondem ao tipo de rasterização GL_LINES.
  g_VirtualScene["axes"] = axes;

  // Criamos um buffer OpenGL para armazenar os índices acima
  GLuint indices_id;
  glGenBuffers(1, &indices_id);

  // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

  // Alocamos memória para o buffer.
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

  // Copiamos os valores do array indices[] para dentro do buffer.
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

  // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
  // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
  // caso contrário o VAO irá perder a informação sobre os índices.
  //
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
  //

  // "Desligamos" o VAO, evitando assim que operações posteriores venham a
  // alterar o mesmo. Isso evita bugs.
  glBindVertexArray(0);

  // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
  // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
  return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char *filename)
{
  // Criamos um identificador (ID) para este shader, informando que o mesmo
  // será aplicado nos vértices.
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

  // Carregamos e compilamos o shader
  LoadShader(filename, vertex_shader_id);

  // Retorna o ID gerado acima
  return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char *filename)
{
  // Criamos um identificador (ID) para este shader, informando que o mesmo
  // será aplicado nos fragmentos.
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  // Carregamos e compilamos o shader
  LoadShader(filename, fragment_shader_id);

  // Retorna o ID gerado acima
  return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char *filename, GLuint shader_id)
{
  // Lemos o arquivo de texto indicado pela variável "filename"
  // e colocamos seu conteúdo em memória, apontado pela variável
  // "shader_string".
  std::ifstream file;
  try
  {
    file.exceptions(std::ifstream::failbit);
    file.open(filename);
  }
  catch (std::exception &e)
  {
    fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
    std::exit(EXIT_FAILURE);
  }
  std::stringstream shader;
  shader << file.rdbuf();
  std::string str = shader.str();
  const GLchar *shader_string = str.c_str();
  const GLint shader_string_length = static_cast<GLint>(str.length());

  // Define o código do shader GLSL, contido na string "shader_string"
  glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

  // Compila o código do shader GLSL (em tempo de execução)
  glCompileShader(shader_id);

  // Verificamos se ocorreu algum erro ou "warning" durante a compilação
  GLint compiled_ok;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

  GLint log_length = 0;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

  // Alocamos memória para guardar o log de compilação.
  // A chamada "new" em C++ é equivalente ao "malloc()" do C.
  GLchar *log = new GLchar[log_length];
  glGetShaderInfoLog(shader_id, log_length, &log_length, log);

  // Imprime no terminal qualquer erro ou "warning" de compilação
  if (log_length != 0)
  {
    std::string output;

    if (!compiled_ok)
    {
      output += "ERROR: OpenGL compilation of \"";
      output += filename;
      output += "\" failed.\n";
      output += "== Start of compilation log\n";
      output += log;
      output += "== End of compilation log\n";
    }
    else
    {
      output += "WARNING: OpenGL compilation of \"";
      output += filename;
      output += "\".\n";
      output += "== Start of compilation log\n";
      output += log;
      output += "== End of compilation log\n";
    }

    fprintf(stderr, "%s", output.c_str());
  }

  // A chamada "delete" em C++ é equivalente ao "free()" do C
  delete[] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
  // Criamos um identificador (ID) para este programa de GPU
  GLuint program_id = glCreateProgram();

  // Definição dos dois shaders GLSL que devem ser executados pelo programa
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);

  // Linkagem dos shaders acima ao programa
  glLinkProgram(program_id);

  // Verificamos se ocorreu algum erro durante a linkagem
  GLint linked_ok = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

  // Imprime no terminal qualquer erro de linkagem
  if (linked_ok == GL_FALSE)
  {
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar *log = new GLchar[log_length];

    glGetProgramInfoLog(program_id, log_length, &log_length, log);

    std::string output;

    output += "ERROR: OpenGL linking of program failed.\n";
    output += "== Start of link log\n";
    output += log;
    output += "\n== End of link log\n";

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete[] log;

    fprintf(stderr, "%s", output.c_str());
  }

  // Retornamos o ID gerado acima
  return program_id;
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
