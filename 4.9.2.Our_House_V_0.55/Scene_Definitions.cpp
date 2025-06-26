#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"


unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

bool fuckcat = false;

std::vector<Light_Parameters> lightList;
// 추가
void initLights() {
	lightList.clear();
	Light_Parameters world = {
	1,                                        // light on
	{0.0f, 0.0f, 100.0f, 1.0f},               // position
	{0.0f, 0.1f, 0.1f, 1.0f},                 // ambient_color 
	{0.0f, 1.0f, 1.0f, 1.0f},                 // diffuse_color 
	{0.0f, 1.0f, 1.0f, 1.0f},                 // specular_color 
	{0.0f, 0.0f, -1.0f},                      // spot_direction
	0.0f,                                     // spot_exponent (집중도)
	180.0f * TO_RADIAN,                       // spot_cutoff_angle (각도)
	{1.0f, 0.0f, 0.0f}                  // attenuation (constant만 사용)
	};
	lightList.push_back(world);

	Light_Parameters eye = {
	0,                                        // light on
	{0.0f, 0.0f, 100.0f, 1.0f},               // position
	{0.1f, 0.1f, 0.1f, 1.0f},                 // ambient_color (white)
	{0.9f, 0.9f, 0.9f, 1.0f},                 // diffuse_color (white)
	{0.9f, 0.9f, 0.9f, 1.0f},                 // specular_color (white)
	{0.0f, 0.0f, -1.0f},                      // spot_direction
	3.0f,                                     // spot_exponent (집중도)
	30.0f * TO_RADIAN,                         // spot_cutoff_angle (각도)
	{1.0f, 0.005f, 0.0f}                  // attenuation (constant만 사용)
	};
	lightList.push_back(eye);

	Light_Parameters model = {
	1,                                        // light off
	{0.0f, 0.0f, 100.0f, 1.0f},               // position
	{0.1f, 0.1f, 0.1f, 1.0f},                 // ambient_color (white)
	{1.0f, 0.1f, 0.1f, 1.0f},                 // diffuse_color (white)
	{1.0f, 0.1f, 0.1f, 1.0f},                 // specular_color (white)
	{0.0f, 0.0f, -1.0f},                      // spot_direction
	4.0f,                                     // spot_exponent (집중도)
	40.0f * TO_RADIAN,                         // spot_cutoff_angle (각도)
	{1.0f, 0.0f, 0.0f}                  // attenuation (constant만 사용)
	};
	lightList.push_back(model);

	Light_Parameters comp = {
	0,                                        // light off
	{-35.0f, -55.0f, 110.0f, 1.0f},               // position
	{0.1f, 0.1f, 0.1f, 1.0f},                 // ambient_color (white)
	{0.9f, 0.9f, 0.9f, 1.0f},                 // diffuse_color (white)
	{1.0f, 1.0f, 1.0f, 1.0f},                 // specular_color (white)
	{0.0f, 0.0f, -1.0f},                      // spot_direction
	2.0f,                                     // spot_exponent (집중도)
	40.0f * TO_RADIAN,                         // spot_cutoff_angle (각도)
	{1.0f, 0.005f, 0.0f}                  // attenuation (constant만 사용)
	};
	lightList.push_back(comp);
}

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(glm::mat4& ModelMatrix, Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {

	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.table.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.table);

	static_geometry_data.light.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.light);

	static_geometry_data.teapot.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.teapot);

	static_geometry_data.new_chair.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_chair);

	static_geometry_data.frame.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.frame);

	static_geometry_data.new_picture.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_picture);

	static_geometry_data.cow.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.cow);


	// NEW
	static_geometry_data.optimus.define_object();
	static_object_ID_mapper[STATIC_OBJECT_OPTIMUS] = static_objects.size();
	static_objects.push_back(static_geometry_data.optimus);

	static_geometry_data.helicopter.define_object();
	static_object_ID_mapper[STATIC_OBJECT_HELICOPTER] = static_objects.size();
	static_objects.push_back(static_geometry_data.helicopter);

	static_geometry_data.cat.define_object();
	static_object_ID_mapper[STATIC_OBJECT_CAT] = static_objects.size();
	static_objects.push_back(static_geometry_data.cat);

	static_geometry_data.tower.define_object();
	static_object_ID_mapper[STATIC_OBJECT_TOWER] = static_objects.size();
	static_objects.push_back(static_geometry_data.tower);

	static_geometry_data.ironman.define_object();
	static_object_ID_mapper[STATIC_OBJECT_IRONMAN] = static_objects.size();
	static_objects.push_back(static_geometry_data.ironman);
}

void Scene::build_dynamic_world() {
	dynamic_geometry_data.tiger_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.tiger_d);

	// two different moving cows
	dynamic_geometry_data.cow_d_1.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_1] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.cow_d_1);

	dynamic_geometry_data.cow_d_2.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_2] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.cow_d_2);
	
	dynamic_geometry_data.Ben_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_BEN] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.Ben_d);

	dynamic_geometry_data.spider_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_SPIDER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.spider_d);
}

void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();
	// main camera
	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);

	// side camera
	camera_data.cam_side.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_SIDE] = camera_list.size();
	camera_list.push_back(camera_data.cam_side);

	// side front camera
	camera_data.cam_side_front.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_SIDE_FRONT] = camera_list.size();
	camera_list.push_back(camera_data.cam_side_front);

	// Top camera
	camera_data.cam_top.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_TOP] = camera_list.size();
	camera_list.push_back(camera_data.cam_top);

	// CC0
	camera_data.cam_cc0.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_0] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc0);

	// CC1
	camera_data.cam_cc1.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_1] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc1);

	// CC2
	camera_data.cam_cc2.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_2] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc2);

	//Dynamic Cam
	camera_data.cam_dynamic.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_DYNAMIC] = camera_list.size();
	camera_list.push_back(camera_data.cam_dynamic);
	// define others here


	// ORTHO
	// Ortho X
	camera_data.cam_ortho_x.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_ORTHO_X] = camera_list.size();
	camera_list.push_back(camera_data.cam_ortho_x);

	// Ortho Y
	camera_data.cam_ortho_y.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_ORTHO_Z] = camera_list.size();
	camera_list.push_back(camera_data.cam_ortho_y);

	// Ortho Z
	camera_data.cam_ortho_z.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_ORTHO_Z] = camera_list.size();
	camera_list.push_back(camera_data.cam_ortho_z);

	
}

void Scene::build_shader_list() {
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);

	// 추가
	shader_data.shader_phong.prepare_shader();
	shader_ID_mapper[SHADER_PHONG] = shader_list.size();
	shader_list.push_back(shader_data.shader_phong);

	shader_data.shader_gouraud.prepare_shader();
	shader_ID_mapper[SHADER_GOURAUD] = shader_list.size();
	shader_list.push_back(shader_data.shader_gouraud);

	shader_data.shader_phong_texture.prepare_shader();
	shader_ID_mapper[SHADER_PHONG_TEXTURE] = shader_list.size();
	shader_list.push_back(shader_data.shader_phong_texture);

	initLights();
}

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();
}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto static_object = static_objects.begin(); static_object != static_objects.end(); static_object++) {
		if (static_object->get().flag_valid == false) continue;
		static_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list);
	}
}

void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto dynamic_object = dynamic_objects.begin(); dynamic_object != dynamic_objects.end(); dynamic_object++) {
		if (dynamic_object->get().flag_valid == false) continue;
		if (dynamic_object->get().object_id == DYNAMIC_OBJECT_BEN) {
			dynamic_object->get().draw_object(DynamicModelMatrixes[0], ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
		}
		else if (dynamic_object->get().object_id == DYNAMIC_OBJECT_SPIDER) {
			dynamic_object->get().draw_object(DynamicModelMatrixes[1], ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
		}
		else { // Not use first parameter
			dynamic_object->get().draw_object(DynamicModelMatrixes[0], ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
		}
	}
}

void Scene::draw_axis() {
	if (axistoggle) {
		axis_object.draw_axis(AxisMatrix, static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
			ViewMatrix, ProjectionMatrix);
	}

	// This is for cctv unv Axis.
	else {
		for (int i = 0; i < CC_axis_object.size(); i++) {
			CC_axis_object[i].draw_axis(CCAxisModelMatrixes[i], static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
				ViewMatrix, ProjectionMatrix);
		}
	}
}

void Scene::draw_world() {
	draw_axis();
	draw_static_world();
	draw_dynamic_world();
}