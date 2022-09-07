#shader vertex
#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

// NOTE: These attributes are not required for the challenges
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 fTexCoords;

// NOTE: These two variables are not required for the challenges
out vec3 fNormal;
out mat3 fTBN;

uniform mat4 uTransform;
uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    fTexCoords = aTexCoords;

    // NOTE: This block is not required for the challenges
    fNormal = aNormal;
    vec3 T = normalize(vec3(vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(vec4(aNormal,    0.0)));
    fTBN = mat3(T, B, N);

    gl_Position = uProjection * uView * (uTransform * vec4(aPosition, 1.0));
};

#shader fragment
#version 460 core

out vec4 FragColor;

in vec2 fTexCoords;

// NOTE: These are not required for the challenges
in vec3 fNormal;
in mat3 fTBN;

uniform int uIsOpaque;
uniform sampler2D uTexture;

// NOTE: These are not required for the challenges
uniform sampler2D uNormalTexture;
const vec3 sunlightDir = normalize(vec3(-0.3, -0.5, -0.2));
const float ambientOcclusion = 0.3f;

void main()
{
    // NOTE: For the challenge all you need here is something like
    // FragColor = texture(uTexture, fTexCoords);
    // Everything in this shader is just to enable normal surface lighting :)

    vec4 defaultColor = vec4(0.6, 0.1, 0.04, 1.0);
    vec4 albedo = texture(uTexture, fTexCoords);
    float lightStrength = clamp(-dot(fNormal, sunlightDir), 0, 1);
    // float lightStrength = 1;
    lightStrength = max(ambientOcclusion, lightStrength);

    vec3 textureNormal = normalize(texture(uNormalTexture, fTexCoords).xyz * 2.0 - 1.0);
    textureNormal = normalize(fTBN * textureNormal);
    float incidentAngle = -dot(textureNormal, sunlightDir);
    // float normalLightStrength = clamp(incidentAngle, 0.0, 1.0);
    float normalLightStrength = 1;
    normalLightStrength = (lightStrength * 0.7) + 0.3;

    if (incidentAngle < 0) {
        normalLightStrength -= ((incidentAngle * 0.7) + 0.3);
        normalLightStrength = clamp(normalLightStrength, 0, 1);
    }

    if (uIsOpaque == 0) {
        normalLightStrength = 1;
    }

    vec4 finalColor = albedo * normalLightStrength;

    FragColor = finalColor;
};