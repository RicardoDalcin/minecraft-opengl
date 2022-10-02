#shader vertex
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

out vec2 fTextureCoord;
out vec3 fNormal;

void main()
{
    fTextureCoord = aTextureCoord;
    fNormal = aNormal;

    gl_Position = uProjection * uView * (uTransform * vec4(aPosition, 1.0));
};

#shader fragment
#version 330 core

in vec2 fTextureCoord;
in vec3 fNormal;

uniform int uIsOpaque;
uniform sampler2D uTexture;

out vec4 FragColor;

const vec3 lightSourceDirection = normalize(vec3(-0.3, -0.5, -0.2));
const float minLightStrength = 0.3f;

void main()
{
    vec4 textureColor = texture(uTexture, fTextureCoord);

    float lightStrength = clamp(-dot(fNormal, lightSourceDirection), 0, 1);
    lightStrength = max(minLightStrength, lightStrength);

    vec3 textureNormal = normalize(normalize(vec3(vec4(fNormal,    0.0))));
    float incidentAngle = -dot(textureNormal, lightSourceDirection);
    
    float normalLightStrength = (lightStrength * 0.7) + 0.3;

    if (incidentAngle < 0) {
        normalLightStrength -= ((incidentAngle * 0.7) + 0.3);
        normalLightStrength = clamp(normalLightStrength, 0, 1);
    }

    if (uIsOpaque == 0) {
        normalLightStrength = 1;
    }

    FragColor = textureColor * normalLightStrength;
};