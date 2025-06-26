#version 330 core
#define MAX_LIGHTS 8

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat3 u_normalMatrix;

uniform vec3  u_viewPos;

// light uniforms (mirror what Phong.vert uses)
uniform int   uLightOn[MAX_LIGHTS];
uniform vec4  uLightPos[MAX_LIGHTS];
uniform vec4  uLightAmbient[MAX_LIGHTS];
uniform vec4  uLightDiffuse[MAX_LIGHTS];
uniform vec4  uLightSpecular[MAX_LIGHTS];
uniform vec3  uLightSpotDirection[MAX_LIGHTS];
uniform float uLightSpotExponent[MAX_LIGHTS];
uniform float uLightSpotCutoffAngle[MAX_LIGHTS];
uniform vec3  uAttenuation[MAX_LIGHTS];

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material uMaterial;

out vec4 vertexColor;

void main() {
    // Transform position & normal
    vec3 WorldPos = vec3(u_model * vec4(aPos, 1.0));
    vec3 N = normalize(u_normalMatrix * aNormal);
    vec3 V = normalize(u_viewPos - WorldPos);

    vec3 color = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (uLightOn[i] == 1) {
            vec3 L = normalize(uLightPos[i].xyz - WorldPos);

            // ambient
            vec3 amb = vec3(uLightAmbient[i]) * vec3(uMaterial.ambient);

            // diffuse
            float diff = max(dot(N, L), 0.0);
            vec3 dif = diff * vec3(uLightDiffuse[i]) * vec3(uMaterial.diffuse);

            // specular
            vec3 R = reflect(-L, N);
            float spec = pow(max(dot(V, R), 0.0), uMaterial.shininess);
            vec3 sp = spec * vec3(uLightSpecular[i]) * vec3(uMaterial.specular);

            // spotlight
            float theta = dot(-L, normalize(uLightSpotDirection[i]));
            float cutoff = cos(uLightSpotCutoffAngle[i]);
            float spot = (theta > cutoff) ? pow(theta, uLightSpotExponent[i]) : 0.0;

            // attenuation
            float d = length(uLightPos[i].xyz - WorldPos);
            vec3 att = uAttenuation[i];
            float atten = 1.0 / (att.x + att.y * d + att.z * d * d);

            color += amb + (dif + sp) * spot * atten;
        }
    }

    vertexColor = vec4(color, 1.0);
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
}
