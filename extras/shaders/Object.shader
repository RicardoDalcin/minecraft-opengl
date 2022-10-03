#shader vertex
#version 330 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform int uIsGouraud;

varying vec4 fGouraudColor;

out vec4 fPositionWorld;
out vec4 fNormal;

void main()
{
    fPositionWorld = uModel * model_coefficients;

    fNormal = inverse(transpose(uModel)) * normal_coefficients;
    fNormal.w = 0.0;

    if (uIsGouraud == 1) {
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(uView) * origin;

        vec4 p = fPositionWorld;

        vec4 n = normalize(fNormal);
        vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));
        vec4 v = normalize(camera_position - p);

        vec4 r = -l + 2 * n * dot(n, l);

        vec3 Kd = vec3(0.8,0.4,0.08);
        vec3 Ks = vec3(0.0,0.0,0.0);
        vec3 Ka = Kd / 2;
        float q = 1.0;

        vec3 I = vec3(1.0,1.0,1.0);

        vec3 Ia = vec3(0.2,0.2,0.2);

        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        vec3 ambient_term = Ka * Ia;

        vec3 phong_specular_term  = Ks * I * pow(max(0, dot(r, v)), q);

        fGouraudColor.a = 1;

        fGouraudColor.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

        fGouraudColor.rgb = pow(fGouraudColor.rgb, vec3(1.0,1.0,1.0) / 2.2);    
    }

    gl_Position = uProjection * uView * uModel * model_coefficients;

};

#shader fragment
#version 330 core

in vec4 fPositionWorld;
in vec4 fNormal;

varying vec4 fGouraudColor;

uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uProjection;

uniform int uIsGouraud;

out vec4 FragColor;

void main()
{
    if (uIsGouraud == 1) {
        FragColor = fGouraudColor;
    } else {
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(uView) * origin;

        vec4 p = fPositionWorld;

        vec4 n = normalize(fNormal);
        vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));
        vec4 v = normalize(camera_position - p);

        vec4 r = -l + 2 * n * dot(n, l);

        vec3 Kd = vec3(0.8,0.4,0.08);
        vec3 Ks = vec3(0.0,0.0,0.0);
        vec3 Ka = Kd / 2;
        float q = 1.0;

        vec3 I = vec3(1.0,1.0,1.0);

        vec3 Ia = vec3(0.2,0.2,0.2);

        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        vec3 ambient_term = Ka * Ia;

        vec3 phong_specular_term  = Ks * I * pow(max(0, dot(r, v)), q);

        FragColor.a = 1;

        FragColor.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

        FragColor.rgb = pow(FragColor.rgb, vec3(1.0,1.0,1.0) / 2.2);  
    }
};