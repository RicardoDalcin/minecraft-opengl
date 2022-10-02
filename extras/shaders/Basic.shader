#shader vertex
#version 330 core

layout (location = 0) in vec4 aPosition;

uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    gl_Position = uProjection * uView * uTransform * aPosition;
};

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.3, 0.7, 1.0);
};