#ifndef _VERTEXARRAY_H
#define _VERTEXARRAY_H

#include "VertexBuffer.hpp"

class VertexBufferLayout;

// Classe para gerenciamento de vertex array object
class VertexArray
{
private:
  unsigned int m_RendererId;

public:
  VertexArray();
  ~VertexArray();

  void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

  void Bind() const;
  void Unbind() const;
};

#endif
