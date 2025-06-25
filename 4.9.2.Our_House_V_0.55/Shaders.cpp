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

    // 3) Grab all the uniform locations declared in Phong.vert / Phong.frag
    //    (model, view, projection, normal matrix)
    loc_uModel = glGetUniformLocation(h_ShaderProgram, "u_model");
    loc_uView = glGetUniformLocation(h_ShaderProgram, "u_view");
    loc_uProjection = glGetUniformLocation(h_ShaderProgram, "u_projection");
    loc_uNormalMatrix = glGetUniformLocation(h_ShaderProgram, "u_normalMatrix");

    //    camera position
    loc_uViewPos = glGetUniformLocation(h_ShaderProgram, "u_viewPos");

    //    light parameters
    loc_light_on = glGetUniformLocation(h_ShaderProgram, "u_light.light_on");
    loc_light_pos = glGetUniformLocation(h_ShaderProgram, "u_light.position");
    loc_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_light.ambient_color");
    loc_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_light.diffuse_color");
    loc_light_specular = glGetUniformLocation(h_ShaderProgram, "u_light.specular_color");
    loc_spot_direction = glGetUniformLocation(h_ShaderProgram, "u_light.spot_direction");
    loc_spot_exponent = glGetUniformLocation(h_ShaderProgram, "u_light.spot_exponent");
    loc_spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram, "u_light.spot_cutoff_angle");
    loc_light_attenuation = glGetUniformLocation(h_ShaderProgram, "u_light.light_attenuation_factors");

    //    material parameters
    loc_mat_ambient = glGetUniformLocation(h_ShaderProgram, "u_material.ambient_color");
    loc_mat_diffuse = glGetUniformLocation(h_ShaderProgram, "u_material.diffuse_color");
    loc_mat_specular = glGetUniformLocation(h_ShaderProgram, "u_material.specular_color");
    loc_mat_emissive = glGetUniformLocation(h_ShaderProgram, "u_material.emissive_color");
    loc_mat_shininess = glGetUniformLocation(h_ShaderProgram, "u_material.specular_exponent");

    // 4) Unbind program until draw time
    glUseProgram(0);
}