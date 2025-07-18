#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#include <vector>






// 안써요
void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = false;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Cow_D::define_object() {
#define N_FRAMES_COW_1 1
#define N_FRAMES_COW_2 1
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = false;
	switch (object_id) {

		int n_frames;
	case DYNAMIC_OBJECT_COW_1:
		n_frames = N_FRAMES_COW_1;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();
			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
			cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
			cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
			cur_material->exponent = 128.0f * 0.21794872f;
		}
		break;
	case DYNAMIC_OBJECT_COW_2:
		n_frames = N_FRAMES_COW_2;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
			cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
			cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
			cur_material->exponent = 128.0f * 0.6f;
		}
		break;
	}
}



#define N_BEN_FRAMES 30

void Ben_D::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;

	// Ben 애니메이션에는 모두 GL_CW 를 사용한다고 가정
	flag_valid = true;

	for (int i = 0; i < N_BEN_FRAMES; i++) {
		// 1) 새로운 Static_Object 객체를 벡터에 추가
		object_frames.emplace_back();

		// 2) 각 프레임의 .geom 파일 경로 설정
		sprintf(object_frames[i].filename,
			"Data/dynamic_objects/ben/ben_vntm_%d%d.geom",
			i / 10, i % 10);

		// 3) vertex 데이터 형식(필드 수)과 프런트 페이스 모드 지정
		object_frames[i].n_fields = 8;            // (vertex(3) + normal(3) + texcoord(2))
		object_frames[i].front_face_mode = GL_CW; // CW 모드

		// 4) Static_Object 의 기존 메서드를 이용해 버퍼와 VAO 생성
		object_frames[i].prepare_geom_of_static_object();

		// 5) 인스턴스 하나 추가 → ModelMatrix, Material 설정
		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);

		
		*cur_MM = glm::mat4(1.0f);
		*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		*cur_MM = glm::scale(*cur_MM, glm::vec3(60.0f));
		//*cur_MM = glm::translate(*cur_MM, glm::vec3(32.5f, 42.5f, 0.0f));
		cur_material = &(object_frames[i].instances.back().material);
		// Tiger_D와 동일한 머티리얼 속성 예시 (주황색 톤)
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.3f, 0.15f, 0.05f, 1.0f);
		cur_material->diffuse = glm::vec4(0.8f, 0.5f, 0.2f, 1.0f);
		cur_material->specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		cur_material->exponent = 128.0f * 0.3f;
	}
}



#define N_SPIDER_FRAMES 16

void Spider_D::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;

	// Ben 애니메이션에는 모두 GL_CW 를 사용한다고 가정
	flag_valid = true;

	for (int i = 0; i < N_SPIDER_FRAMES; i++) {
		// 1) 새로운 Static_Object 객체를 벡터에 추가
		object_frames.emplace_back();

		// 2) 각 프레임의 .geom 파일 경로 설정
		sprintf(object_frames[i].filename,
			"Data/dynamic_objects/spider/spider_vnt_%d%d.geom",
			i / 10, i % 10);

		// 3) vertex 데이터 형식(필드 수)과 프런트 페이스 모드 지정
		object_frames[i].n_fields = 8;            // (vertex(3) + normal(3) + texcoord(2))
		object_frames[i].front_face_mode = GL_CW; // CW 모드

		// 4) Static_Object 의 기존 메서드를 이용해 버퍼와 VAO 생성
		object_frames[i].prepare_geom_of_static_object();

		// 5) 인스턴스 하나 추가 → ModelMatrix, Material 설정
		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);

		*cur_MM = glm::mat4(1.0f);
		*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		*cur_MM = glm::rotate(*cur_MM, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		*cur_MM = glm::scale(*cur_MM, glm::vec3(17.0f));
		cur_material = &(object_frames[i].instances.back().material);
		// Tiger_D와 동일한 머티리얼 속성 예시 (주황색 톤)
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.3f, 0.15f, 0.05f, 1.0f);
		cur_material->diffuse = glm::vec4(0.8f, 0.5f, 0.2f, 1.0f);
		cur_material->specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		cur_material->exponent = 128.0f * 0.3f;
	}
}




void Dynamic_Object::draw_object(glm::mat4&MM, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	switch (object_id) {
	case DYNAMIC_OBJECT_TIGER:
		rotation_angle = (time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 0.0f, 0.0f));
		break;
	case DYNAMIC_OBJECT_COW_1:
		rotation_angle = (2 * time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	case DYNAMIC_OBJECT_COW_2:
		rotation_angle = (5 * time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 50.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case DYNAMIC_OBJECT_BEN:
		
		ModelMatrix = MM;
		//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(20.0f, 37.5f, 0.0f));
		break;
	case DYNAMIC_OBJECT_SPIDER:
		
		ModelMatrix = MM;
		//ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	}
	

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}




