#ifndef _SHADER_H
#define _SHADER_H

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

struct ShaderProgramSource
{
  std::string VertexSource;
  std::string FragmentSource;
};

// Classe para load, compilação e uso de shaders
class Shader
{
private:
  unsigned int m_RendererId;
  std::string m_FilePath;
  std::unordered_map<std::string, int> m_UniformLocationCache;

public:
  Shader(const std::string &filePath);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  void SetUniform1i(const std::string &name, int value);
  void SetUniform1f(const std::string &name, float value);
  void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);
  void SetUniformMat4f(const std::string &name, const glm::mat4 matrix);

private:
  ShaderProgramSource ParseShader(const std::string &filePath);

  unsigned int CompileShader(unsigned int type, const std::string &source);
  unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);

  int GetUniformLocation(const std::string &name);
};

#endif
