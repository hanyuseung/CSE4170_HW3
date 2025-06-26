#include "Scene_Definitions.h"

void Shader_Simple::prepare_shader() {
	shader_info[0] = { GL_VERTEX_SHADER, "Shaders/simple.vert" };
	shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/simple.frag" };
	shader_info[2] = { GL_NONE, NULL };

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
	glUseProgram(0);
}
void Shader_Phong::prepare_shader() {
    // 1) Specify the two shader stages and terminate array
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/Phong.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/Phong.frag" };
    shader_info[2] = { GL_NONE, NULL };

    // 2) Compile, link, and use the program
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);

    // 3) uniform --> fragsh.
    loc_uModel = glGetUniformLocation(h_ShaderProgram, "uModel");
    loc_uView = glGetUniformLocation(h_ShaderProgram, "uView");
    loc_uProjection = glGetUniformLocation(h_ShaderProgram, "uProjection");
    loc_uNormalMatrix = glGetUniformLocation(h_ShaderProgram, "uNormalMatrix");

    loc_uLightOn = glGetUniformLocation(h_ShaderProgram, "uLightOn");
    loc_uViewPos = glGetUniformLocation(h_ShaderProgram, "uViewPos");

    loc_uAttenuation = glGetUniformLocation(h_ShaderProgram, "uAttenuation");

    loc_uNumLights = glGetUniformLocation(h_ShaderProgram, "uNumLights");
    loc_uLightPos = glGetUniformLocation(h_ShaderProgram, "uLightPos");
    loc_uLightAmbient = glGetUniformLocation(h_ShaderProgram, "uLightAmbient");
    loc_uLightDiffuse = glGetUniformLocation(h_ShaderProgram, "uLightDiffuse");
    loc_uLightSpecular = glGetUniformLocation(h_ShaderProgram, "uLightSpecular");

    loc_uLightSpotDirection = glGetUniformLocation(h_ShaderProgram, "uLightSpotDirection");
    loc_uLightSpotExponent = glGetUniformLocation(h_ShaderProgram, "uLightSpotExponent");
    loc_uLightSpotCutoffAngle = glGetUniformLocation(h_ShaderProgram, "uLightSpotCutoffAngle");


    loc_uMatAmbient = glGetUniformLocation(h_ShaderProgram, "uMaterial.ambient");
    loc_uMatDiffuse = glGetUniformLocation(h_ShaderProgram, "uMaterial.diffuse");
    loc_uMatSpecular = glGetUniformLocation(h_ShaderProgram, "uMaterial.specular");
    loc_uMatShininess = glGetUniformLocation(h_ShaderProgram, "uMaterial.shininess");

    // 4) Unbind program until draw time
    glUseProgram(0);
}


void Shader_Gouraud::prepare_shader() {
    // 1) Specify the two shader stages and terminate array
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/Gouraud.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" };
    shader_info[2] = { GL_NONE, NULL };

    // 2) Compile, link, and use the program
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);

    // 3) uniform --> fragsh.
    loc_uModel = glGetUniformLocation(h_ShaderProgram, "u_model");
    loc_uView = glGetUniformLocation(h_ShaderProgram, "u_view");
    loc_uProjection = glGetUniformLocation(h_ShaderProgram, "u_projection");
    loc_uNormalMatrix = glGetUniformLocation(h_ShaderProgram, "u_normalMatrix");
    loc_uViewPos = glGetUniformLocation(h_ShaderProgram, "u_viewPos");


    loc_uLightOn = glGetUniformLocation(h_ShaderProgram, "uLightOn");
    loc_uAttenuation = glGetUniformLocation(h_ShaderProgram, "uAttenuation");

    loc_uNumLights = glGetUniformLocation(h_ShaderProgram, "uNumLights");
    loc_uLightPos = glGetUniformLocation(h_ShaderProgram, "uLightPos");
    loc_uLightAmbient = glGetUniformLocation(h_ShaderProgram, "uLightAmbient");
    loc_uLightDiffuse = glGetUniformLocation(h_ShaderProgram, "uLightDiffuse");
    loc_uLightSpecular = glGetUniformLocation(h_ShaderProgram, "uLightSpecular");

    loc_uLightSpotDirection = glGetUniformLocation(h_ShaderProgram, "uLightSpotDirection");
    loc_uLightSpotExponent = glGetUniformLocation(h_ShaderProgram, "uLightSpotExponent");
    loc_uLightSpotCutoffAngle = glGetUniformLocation(h_ShaderProgram, "uLightSpotCutoffAngle");


    loc_uMatAmbient = glGetUniformLocation(h_ShaderProgram, "uMaterial.ambient");
    loc_uMatDiffuse = glGetUniformLocation(h_ShaderProgram, "uMaterial.diffuse");
    loc_uMatSpecular = glGetUniformLocation(h_ShaderProgram, "uMaterial.specular");
    loc_uMatShininess = glGetUniformLocation(h_ShaderProgram, "uMaterial.shininess");

    // 4) Unbind program until draw time
    glUseProgram(0);
}

void Shader_Phong_Texture::prepare_shader() {
    // 1) Specify the two shader stages and terminate array
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/PhongTx.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/PhongTx.frag" };
    shader_info[2] = { GL_NONE, NULL };

    // 2) Compile, link, and use the program
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);


    loc_uModel = glGetUniformLocation(h_ShaderProgram, "uModel");
    loc_uView = glGetUniformLocation(h_ShaderProgram, "uView");
    loc_uProjection = glGetUniformLocation(h_ShaderProgram, "uProjection");
    loc_uNormalMatrix = glGetUniformLocation(h_ShaderProgram, "uNormalMatrix");

    loc_uViewPos = glGetUniformLocation(h_ShaderProgram, "uViewPos");

    loc_uAttenuation = glGetUniformLocation(h_ShaderProgram, "uAttenuation");

    loc_uNumLights = glGetUniformLocation(h_ShaderProgram, "uNumLights");
    loc_uLightOn = glGetUniformLocation(h_ShaderProgram, "uLightOn");
    loc_uLightPos = glGetUniformLocation(h_ShaderProgram, "uLightPos");
    loc_uLightAmbient = glGetUniformLocation(h_ShaderProgram, "uLightAmbient");
    loc_uLightDiffuse = glGetUniformLocation(h_ShaderProgram, "uLightDiffuse");
    loc_uLightSpecular = glGetUniformLocation(h_ShaderProgram, "uLightSpecular");

    loc_uLightSpotDirection = glGetUniformLocation(h_ShaderProgram, "uLightSpotDirection");
    loc_uLightSpotExponent = glGetUniformLocation(h_ShaderProgram, "uLightSpotExponent");
    loc_uLightSpotCutoffAngle = glGetUniformLocation(h_ShaderProgram, "uLightSpotCutoffAngle");

    loc_uMatAmbient = glGetUniformLocation(h_ShaderProgram, "uMaterial.ambient");
    loc_uMatDiffuse = glGetUniformLocation(h_ShaderProgram, "uMaterial.diffuse");
    loc_uMatSpecular = glGetUniformLocation(h_ShaderProgram, "uMaterial.specular");
    loc_uMatShininess = glGetUniformLocation(h_ShaderProgram, "uMaterial.shininess");

    loc_uTexture = glGetUniformLocation(h_ShaderProgram, "uTexture");
}