#ifndef _VERTEXBUFFER_H
#define _VERTEXBUFFER_H

// Classe para gerenciamento de vertex buffer
class VertexBuffer
{
private:
  unsigned int m_RendererId;

public:
  VertexBuffer(const void *data, unsigned int size);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;
};

#endif
