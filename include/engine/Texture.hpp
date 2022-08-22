#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "Renderer.hpp"

class Texture
{
private:
  unsigned int m_RendererId;
  std::string m_FilePath;
  unsigned char *m_LocalBuffer;
  int m_Width, m_Height, m_BPP;

public:
  Texture(const std::string &filePath, bool pixelated = false);
  ~Texture();

  void Bind(unsigned int slot = 0) const;
  void Unbind() const;

  inline int GetWidth() const { return m_Width; }
  inline int GetHeight() const { return m_Height; }
};

#endif // _TEXTURE_H