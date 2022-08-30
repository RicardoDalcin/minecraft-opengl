#shader vertex
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 fTexCoords;

void main()
{
    fTexCoords = aTexCoords;
    gl_Position = vec4(aPosition, 1.0);
};

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 fTexCoords;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, fTexCoords);
};