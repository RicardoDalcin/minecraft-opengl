
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "core.h"

#include "engine/Shader.hpp"
#include "engine/Renderer.hpp"

// Classe para load, compilação e uso de shaders
Shader::Shader(const std::string &filepath)
    : m_RendererId(0),
      m_FilePath(filepath)
{
  ShaderProgramSource source = ParseShader(filepath);
  m_RendererId = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
  glDeleteProgram(m_RendererId);
}

ShaderProgramSource Shader::ParseShader(const std::string &filePath)
{
  std::ifstream stream(filePath);

  enum class ShaderType
  {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
  };

  std::string line;
  std::stringstream stringStream[2];

  ShaderType shaderType = ShaderType::NONE;

  while (getline(stream, line))
  {
    if (line.find("#shader") != std::string::npos)
    {
      if (line.find("vertex") != std::string::npos)
        shaderType = ShaderType::VERTEX;
      else if (line.find("fragment") != std::string::npos)
        shaderType = ShaderType::FRAGMENT;
    }
    else
    {
      stringStream[(int)shaderType] << line << '\n';
    }
  }

  return {stringStream[0].str(), stringStream[1].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
  unsigned int id = glCreateShader(type);
  const char *src = source.c_str();

  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE)
  {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));

    glGetShaderInfoLog(id, length, &length, message);

    std::cout << "Failed to compile "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << " shader!" << std::endl;
    std::cout << message << std::endl;

    glDeleteShader(id);

    return 0;
  }

  return id;
}

unsigned int Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
  unsigned int program = glCreateProgram();

  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

void Shader::Bind() const
{
  glUseProgram(m_RendererId);
}

void Shader::Unbind() const
{
  glUseProgram(0);
}

void Shader::SetUniform1i(const std::string &name, int value)
{
  glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string &name, float value)
{
  glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
  glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 matrix)
{
  glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string &name)
{
  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    return m_UniformLocationCache[name];

  int location = glGetUniformLocation(m_RendererId, name.c_str());

  if (location == -1)
    std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

  m_UniformLocationCache[name] = location;
  return location;
}
