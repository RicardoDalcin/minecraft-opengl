#ifndef _OBJECT_H
#define _OBJECT_H

#include "core.h"

// #include "tiny_obj_loader/tiny_obj_loader.h"

#include "engine/VertexArray.hpp"
#include "engine/VertexBuffer.hpp"
#include "engine/VertexBufferLayout.hpp"
#include "engine/Shader.hpp"
#include "engine/Texture.hpp"

#include "core/matrices.hpp"

class Object
{
private:
  float m_ObjectX;
  float m_ObjectZ;

  GLuint m_VertexArrayObjectId;
  GLuint m_VertexBufferObjectId;
  int m_IndexCount;
  int m_FirstIndex;

  tinyobj::attrib_t m_Attributes;
  std::vector<tinyobj::shape_t> m_Shapes;
  std::vector<tinyobj::material_t> m_Materials;

public:
  Object(std::string filename, float objectX, float objectZ);
  ~Object();

  float GetObjectX() const { return m_ObjectX; }
  float GetObjectZ() const { return m_ObjectZ; }

  void ComputeNormals();
  void BuildVertices();

  void Draw(Shader *shader, glm::mat4 view, glm::mat4 projection, bool isGouraud);
};

#endif