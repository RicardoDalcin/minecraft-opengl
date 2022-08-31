#ifndef _RENDERER_H
#define _RENDERER_H

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

class Renderer
{
public:
  void Clear() const;
  void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
  void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader *shader) const;
};

#endif // _RENDERER_H
