// Phong.frag
#version 330 core

in vec3 v_fragPos;
in vec3 v_normal;

out vec4 fragColor;

// Light and material definitions (match My_Shading.h) fileciteturn5file1
struct Light_Parameters {
    int light_on;
    vec4 position;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    vec3 spot_direction;
    float spot_exponent;
    float spot_cutoff_angle;
    vec4 light_attenuation_factors;
};

struct Material_Parameters {
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emissive_color;
    float specular_exponent;
};

uniform Light_Parameters u_light;
uniform Material_Parameters u_material;
uniform vec3 u_viewPos;

void main() {
    if (u_light.light_on == 0) {
        fragColor = u_material.emissive_color + u_material.ambient_color;
        return;
    }

    // Ambient term
    vec3 ambient = (u_light.ambient_color.rgb * u_material.ambient_color.rgb);

    // Diffuse term
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_light.position.xyz - v_fragPos);
    float diffFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_light.diffuse_color.rgb * diffFactor * u_material.diffuse_color.rgb;

    // Specular term
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_material.specular_exponent);
    vec3 specular = u_light.specular_color.rgb * specFactor * u_material.specular_color.rgb;

    // Attenuation
    float distance = length(u_light.position.xyz - v_fragPos);
    vec4 att = u_light.light_attenuation_factors;
    float attenuation = 1.0;
    if (att.w != 0.0) {
        attenuation = 1.0 / (att.x + att.y * distance + att.z * distance * distance + att.w * distance * distance * distance);
    }

    vec3 color = (ambient + diffuse + specular) * attenuation + u_material.emissive_color.rgb;
    fragColor = vec4(color, u_material.diffuse_color.a);
}
