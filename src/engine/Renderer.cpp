#include "core.h"

#include "engine/Renderer.hpp"

#include <iostream>

// Classe para gerenciamento de renderização da aplicação

void GLClearError()
{
  while (glGetError() != GL_NO_ERROR)
    ;
}

bool GLLogCall(const char *function, const char *file, int line)
{
  while (GLenum error = glGetError())
  {
    std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << std::endl;
    return false;
  }

  return true;
}

void Renderer::Clear() const
{
  glClearColor(0.2f, 0.8f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
{
  shader.Bind();
  va.Bind();
  ib.Bind();

  glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
};

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader *shader) const
{
  shader->Bind();
  va.Bind();
  ib.Bind();

  glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
};