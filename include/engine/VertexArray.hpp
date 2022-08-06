#ifndef _VERTEXARRAY_H
#define _VERTEXARRAY_H

#include "VertexBuffer.hpp"

class VertexBufferLayout;

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

#endif // _VERTEXARRAY_H
