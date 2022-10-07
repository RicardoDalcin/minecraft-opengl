#include "core.h"

#include "engine/VertexBuffer.hpp"

#include "engine/Renderer.hpp"

// Classe para gerenciamento de vertex buffer
VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
  glGenBuffers(1, &m_RendererId);
  glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &m_RendererId);
}

void VertexBuffer::Bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
}

void VertexBuffer::Unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
