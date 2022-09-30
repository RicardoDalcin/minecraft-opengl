#shader vertex
#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    gl_Position = uProjection * uView * uTransform * model_coefficients;
};

#shader fragment
#version 330 core

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
};