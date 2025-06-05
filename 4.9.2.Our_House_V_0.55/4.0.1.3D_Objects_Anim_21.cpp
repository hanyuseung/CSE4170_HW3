#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handles to shader programs

// for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_diffuse_texture, loc_flag_texture_mapping;
GLint loc_flag_fog;

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

/******************************** START: objects ********************************/
// texture stuffs
#define TEXTURE_FLOOR				(0)
#define TEXTURE_TIGER				(1)
#define TEXTURE_WOLF				(2)
#define TEXTURE_SPIDER				(3)
#define TEXTURE_DRAGON				(4)
#define TEXTURE_OPTIMUS				(5)
#define TEXTURE_COW					(6)
#define TEXTURE_BUS					(7)
#define TEXTURE_BIKE				(8)
#define TEXTURE_GODZILLA			(9)
#define TEXTURE_IRONMAN				(10)
#define TEXTURE_TANK				(11)
#define TEXTURE_NATHAN				(12)
#define TEXTURE_OGRE				(13)
#define TEXTURE_CAT					(14)
#define TEXTURE_ANT					(15)
#define TEXTURE_TOWER				(16)
#define N_TEXTURES_USED				(17)

GLuint texture_names[N_TEXTURES_USED];
bool flag_texture_mapping = true;

// texture id
#define TEXTURE_ID_DIFFUSE	(0)
#define TEXTURE_ID_NORMAL	(1)

void My_glTexImage2D_from_file(char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	FreeImage_FlipVertical(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

int read_geometry_vnt(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);

	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

// for multiple materials
int read_geometry_vntm(GLfloat** object, int bytes_per_primitive,
	int* n_matrial_indicies, int** material_indicies,
	int* n_materials, char*** diffuse_texture_names,
	Material_Parameters** material_parameters,
	bool* bOnce,
	char* filename) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}

	int n_faces;
	fread(&n_faces, sizeof(int), 1, fp);

	*object = (float*)malloc(n_faces * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...\n", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_faces, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	fread(n_matrial_indicies, sizeof(int), 1, fp);

	int bytes_per_indices = sizeof(int) * 2;
	*material_indicies = (int*)malloc(bytes_per_indices * (*n_matrial_indicies)); // material id, offset
	if (*material_indicies == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...\n", filename);
		return -1;
	}

	fread(*material_indicies, bytes_per_indices, (*n_matrial_indicies), fp);

	if (*bOnce == false) {
		fread(n_materials, sizeof(int), 1, fp);

		*material_parameters = (Material_Parameters*)malloc(sizeof(Material_Parameters) * (*n_materials));
		*diffuse_texture_names = (char**)malloc(sizeof(char*) * (*n_materials));
		for (int i = 0; i < (*n_materials); i++) {
			fread((*material_parameters)[i].ambient_color, sizeof(float), 3, fp); //Ka
			fread((*material_parameters)[i].diffuse_color, sizeof(float), 3, fp); //Kd
			fread((*material_parameters)[i].specular_color, sizeof(float), 3, fp); //Ks
			fread(&(*material_parameters)[i].specular_exponent, sizeof(float), 1, fp); //Ns
			fread((*material_parameters)[i].emissive_color, sizeof(float), 3, fp); //Ke

			(*material_parameters)[i].ambient_color[3] = 1.0f;
			(*material_parameters)[i].diffuse_color[3] = 1.0f;
			(*material_parameters)[i].specular_color[3] = 1.0f;
			(*material_parameters)[i].emissive_color[3] = 1.0f;

			(*diffuse_texture_names)[i] = (char*)malloc(sizeof(char) * 256);
			fread((*diffuse_texture_names)[i], sizeof(char), 256, fp);
		}
		*bOnce = true;
	}

	fclose(fp);

	return n_faces;
}

void set_material(Material_Parameters* material_parameters) {
	// assume ShaderProgram_TXPS is used
	glUniform4fv(loc_material.ambient_color, 1, material_parameters->ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_parameters->diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_parameters->specular_color);
	glUniform1f(loc_material.specular_exponent, material_parameters->specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_parameters->emissive_color);
}

void bind_texture(GLuint tex, int glTextureId, GLuint texture_name) {
	glActiveTexture(GL_TEXTURE0 + glTextureId);
	glBindTexture(GL_TEXTURE_2D, texture_name);
	glUniform1i(tex, glTextureId);
}

// fog stuffs
// you could control the fog parameters interactively: FOG_COLOR, FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE   
int flag_fog;

// for animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int flag_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0, cur_frame_nathan = 0;
float rotation_angle_tiger = 0.0f;

// axes object
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) { // draw coordinate axes
	// initialize vertex buffer object
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// initialize vertex array object
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_axes(void) {
	// assume ShaderProgram_simple is used
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

// floor object
#define TEX_COORD_EXTENT 1.0f
GLuint rectangle_VBO, rectangle_VAO;
GLfloat rectangle_vertices[6][8] = {  // vertices enumerated counterclockwise
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, 0.0f },
	{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, TEX_COORD_EXTENT },
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, TEX_COORD_EXTENT },
	{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, TEX_COORD_EXTENT }
};

Material_Parameters material_floor;

void prepare_floor(void) { // Draw coordinate axes.
	// Initialize vertex buffer object.
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_floor.ambient_color[0] = 0.0f;
	material_floor.ambient_color[1] = 0.05f;
	material_floor.ambient_color[2] = 0.0f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.2f;
	material_floor.diffuse_color[1] = 0.5f;
	material_floor.diffuse_color[2] = 0.2f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.24f;
	material_floor.specular_color[1] = 0.5f;
	material_floor.specular_color[2] = 0.24f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_FLOOR]);

	My_glTexImage2D_from_file("Data/static_objects/checker_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//   float border_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//	 glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_floor(void) {
	set_material(&material_floor);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_FLOOR]);

	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];

Material_Parameters material_tiger;

void prepare_tiger(void) { // vertices enumerated clockwise
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry_vnt(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_tiger.ambient_color[0] = 0.24725f;
	material_tiger.ambient_color[1] = 0.1995f;
	material_tiger.ambient_color[2] = 0.0745f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.75164f;
	material_tiger.diffuse_color[1] = 0.60648f;
	material_tiger.diffuse_color[2] = 0.22648f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.728281f;
	material_tiger.specular_color[1] = 0.655802f;
	material_tiger.specular_color[2] = 0.466065f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 51.2f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f;
	material_tiger.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_TIGER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/tiger/tiger_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_tiger(void) {
	set_material(&material_tiger);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_TIGER]);

	glFrontFace(GL_CW);

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// ben object
#define N_BEN_FRAMES 30
GLuint ben_VBO, ben_VAO;
int ben_n_triangles[N_BEN_FRAMES];
int ben_vertex_offset[N_BEN_FRAMES];
GLfloat* ben_vertices[N_BEN_FRAMES];

int ben_n_material_indices;
int* ben_material_indices[N_BEN_FRAMES];

int ben_n_materials;
char** ben_diffuse_texture;
GLuint* ben_texture_names;

Material_Parameters* material_ben;

void prepare_ben(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, ben_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	bool bOnce = false;
	for (i = 0; i < N_BEN_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/ben/ben_vntm_%d%d.geom", i / 10, i % 10);
		ben_n_triangles[i] = read_geometry_vntm(&ben_vertices[i], n_bytes_per_triangle,
			&ben_n_material_indices,
			&ben_material_indices[i],
			&ben_n_materials,
			&ben_diffuse_texture,
			&material_ben,
			&bOnce,
			filename);

		// assume all geometry files are effective
		ben_n_total_triangles += ben_n_triangles[i];

		if (i == 0)
			ben_vertex_offset[i] = 0;
		else
			ben_vertex_offset[i] = ben_vertex_offset[i - 1] + 3 * ben_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &ben_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glBufferData(GL_ARRAY_BUFFER, ben_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_BEN_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, ben_vertex_offset[i] * n_bytes_per_vertex,
			ben_n_triangles[i] * n_bytes_per_triangle, ben_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_BEN_FRAMES; i++)
		free(ben_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &ben_VAO);
	glBindVertexArray(ben_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	ben_texture_names = (GLuint*)malloc(sizeof(GLuint) * ben_n_materials);
	glGenTextures(ben_n_materials, ben_texture_names);

	for (int i = 0; i < ben_n_materials; i++) {
		if (strcmp("", ben_diffuse_texture[i])) {
			glBindTexture(GL_TEXTURE_2D, ben_texture_names[i]);

			sprintf(filename, "Data/dynamic_objects/ben/%s", ben_diffuse_texture[i]);
			My_glTexImage2D_from_file(filename);

			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void draw_ben(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(ben_VAO);

	for (int i = 0; i < ben_n_material_indices - 1; i++) {
		int cur_material_id = ben_material_indices[cur_frame_ben][2 * i];
		set_material(&material_ben[cur_material_id]);
		bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, ben_texture_names[cur_material_id]);

		int cur_vertex_offset = ben_vertex_offset[cur_frame_ben] + ben_material_indices[cur_frame_ben][2 * i + 1];
		int n_vertices = ben_material_indices[cur_frame_ben][2 * (i + 1) + 1] - ben_material_indices[cur_frame_ben][2 * i + 1];
		glDrawArrays(GL_TRIANGLES, cur_vertex_offset, n_vertices);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);

}

// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

Material_Parameters material_wolf;

void prepare_wolf(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, wolf_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", i);
		wolf_n_triangles[i] = read_geometry_vnt(&wolf_vertices[i], n_bytes_per_triangle, filename);

		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_wolf.ambient_color[0] = 0.5f;
	material_wolf.ambient_color[1] = 0.5f;
	material_wolf.ambient_color[2] = 0.5f;
	material_wolf.ambient_color[3] = 1.0f;

	material_wolf.diffuse_color[0] = 0.500000f;
	material_wolf.diffuse_color[1] = 0.500000f;
	material_wolf.diffuse_color[2] = 0.500000f;
	material_wolf.diffuse_color[3] = 1.0f;

	material_wolf.specular_color[0] = 0.005f;
	material_wolf.specular_color[1] = 0.005f;
	material_wolf.specular_color[2] = 0.005f;
	material_wolf.specular_color[3] = 1.0f;

	material_wolf.specular_exponent = 5.334717f;

	material_wolf.emissive_color[0] = 0.000000f;
	material_wolf.emissive_color[1] = 0.000000f;
	material_wolf.emissive_color[2] = 0.000000f;
	material_wolf.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_WOLF]);

	My_glTexImage2D_from_file("Data/dynamic_objects/wolf/Wolf_Body.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_wolf(void) {
	set_material(&material_wolf);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_WOLF]);

	glFrontFace(GL_CW);

	glBindVertexArray(wolf_VAO);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

Material_Parameters material_spider;

void prepare_spider(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry_vnt(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_spider.ambient_color[0] = 0.5f;
	material_spider.ambient_color[1] = 0.5f;
	material_spider.ambient_color[2] = 0.5f;
	material_spider.ambient_color[3] = 1.0f;

	material_spider.diffuse_color[0] = 0.9f;
	material_spider.diffuse_color[1] = 0.5f;
	material_spider.diffuse_color[2] = 0.1f;
	material_spider.diffuse_color[3] = 1.0f;

	material_spider.specular_color[0] = 0.5f;
	material_spider.specular_color[1] = 0.5f;
	material_spider.specular_color[2] = 0.5f;
	material_spider.specular_color[3] = 1.0f;

	material_spider.specular_exponent = 11.334717f;

	material_spider.emissive_color[0] = 0.000000f;
	material_spider.emissive_color[1] = 0.000000f;
	material_spider.emissive_color[2] = 0.000000f;
	material_spider.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_SPIDER]);

	My_glTexImage2D_from_file("Data/dynamic_objects/spider/Spinnen_Bein_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_spider(void) {
	set_material(&material_spider);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_SPIDER]);

	glFrontFace(GL_CW);

	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// dragon object
GLuint dragon_VBO, dragon_VAO;
int dragon_n_triangles;
GLfloat* dragon_vertices;

Material_Parameters material_dragon;

void prepare_dragon(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, dragon_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/dragon_vnt.geom");
	dragon_n_triangles = read_geometry_vnt(&dragon_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	dragon_n_total_triangles += dragon_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &dragon_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, dragon_VBO);
	glBufferData(GL_ARRAY_BUFFER, dragon_n_total_triangles * 3 * n_bytes_per_vertex, dragon_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(dragon_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &dragon_VAO);
	glBindVertexArray(dragon_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, dragon_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_dragon.ambient_color[0] = 0.5f;
	material_dragon.ambient_color[1] = 0.5f;
	material_dragon.ambient_color[2] = 0.5f;
	material_dragon.ambient_color[3] = 1.0f;

	material_dragon.diffuse_color[0] = 0.3f;
	material_dragon.diffuse_color[1] = 0.3f;
	material_dragon.diffuse_color[2] = 0.4f;
	material_dragon.diffuse_color[3] = 1.0f;

	material_dragon.specular_color[0] = 0.3f;
	material_dragon.specular_color[1] = 0.3f;
	material_dragon.specular_color[2] = 0.4f;
	material_dragon.specular_color[3] = 1.0f;

	material_dragon.specular_exponent = 11.334717f;

	material_dragon.emissive_color[0] = 0.000000f;
	material_dragon.emissive_color[1] = 0.000000f;
	material_dragon.emissive_color[2] = 0.000000f;
	material_dragon.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_DRAGON]);

	My_glTexImage2D_from_file("Data/static_objects/dragon.bmp");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_dragon(void) {
	set_material(&material_dragon);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_DRAGON]);

	glFrontFace(GL_CW);

	glBindVertexArray(dragon_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * dragon_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// optimus object
GLuint optimus_VBO, optimus_VAO;
int optimus_n_triangles;
GLfloat* optimus_vertices;

Material_Parameters material_optimus;

void prepare_optimus(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, optimus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/optimus_vnt.geom");
	optimus_n_triangles = read_geometry_vnt(&optimus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	optimus_n_total_triangles += optimus_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &optimus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glBufferData(GL_ARRAY_BUFFER, optimus_n_total_triangles * 3 * n_bytes_per_vertex, optimus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(optimus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &optimus_VAO);
	glBindVertexArray(optimus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, optimus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_optimus.ambient_color[0] = 0.5f;
	material_optimus.ambient_color[1] = 0.5f;
	material_optimus.ambient_color[2] = 0.5f;
	material_optimus.ambient_color[3] = 1.0f;

	material_optimus.diffuse_color[0] = 0.2f;
	material_optimus.diffuse_color[1] = 0.2f;
	material_optimus.diffuse_color[2] = 0.9f;
	material_optimus.diffuse_color[3] = 1.0f;

	material_optimus.specular_color[0] = 1.0f;
	material_optimus.specular_color[1] = 1.0f;
	material_optimus.specular_color[2] = 1.0f;
	material_optimus.specular_color[3] = 1.0f;

	material_optimus.specular_exponent = 52.334717f;

	material_optimus.emissive_color[0] = 0.000000f;
	material_optimus.emissive_color[1] = 0.000000f;
	material_optimus.emissive_color[2] = 0.000000f;
	material_optimus.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_OPTIMUS]);

	My_glTexImage2D_from_file("Data/static_objects/Optimus.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_optimus(void) {
	set_material(&material_optimus);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_OPTIMUS]);

	glFrontFace(GL_CW);

	glBindVertexArray(optimus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * optimus_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// cow object
GLuint cow_VBO, cow_VAO;
int cow_n_triangles;
GLfloat* cow_vertices;

Material_Parameters material_cow;

void prepare_cow(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, cow_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/cow_vnt.geom");
	cow_n_triangles = read_geometry_vnt(&cow_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	cow_n_total_triangles += cow_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &cow_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glBufferData(GL_ARRAY_BUFFER, cow_n_total_triangles * 3 * n_bytes_per_vertex, cow_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(cow_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &cow_VAO);
	glBindVertexArray(cow_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_cow.ambient_color[0] = 0.5f;
	material_cow.ambient_color[1] = 0.5f;
	material_cow.ambient_color[2] = 0.5f;
	material_cow.ambient_color[3] = 1.0f;

	material_cow.diffuse_color[0] = 0.7f;
	material_cow.diffuse_color[1] = 0.5f;
	material_cow.diffuse_color[2] = 0.2f;
	material_cow.diffuse_color[3] = 1.0f;

	material_cow.specular_color[0] = 0.4f;
	material_cow.specular_color[1] = 0.4f;
	material_cow.specular_color[2] = 0.2f;
	material_cow.specular_color[3] = 1.0f;

	material_cow.specular_exponent = 5.334717f;

	material_cow.emissive_color[0] = 0.000000f;
	material_cow.emissive_color[1] = 0.000000f;
	material_cow.emissive_color[2] = 0.000000f;
	material_cow.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_COW]);

	My_glTexImage2D_from_file("Data/static_objects/Cow.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_cow(void) {
	set_material(&material_cow);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_COW]);

	glFrontFace(GL_CW);

	glBindVertexArray(cow_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * cow_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// bike object
GLuint bike_VBO, bike_VAO;
int bike_n_triangles;
GLfloat* bike_vertices;

Material_Parameters material_bike;

void prepare_bike(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, bike_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bike_vnt.geom");
	bike_n_triangles = read_geometry_vnt(&bike_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bike_n_total_triangles += bike_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bike_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glBufferData(GL_ARRAY_BUFFER, bike_n_total_triangles * 3 * n_bytes_per_vertex, bike_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bike_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bike_VAO);
	glBindVertexArray(bike_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_bike.ambient_color[0] = 0.5f;
	material_bike.ambient_color[1] = 0.5f;
	material_bike.ambient_color[2] = 0.5f;
	material_bike.ambient_color[3] = 1.0f;

	material_bike.diffuse_color[0] = 0.800000f;
	material_bike.diffuse_color[1] = 0.800000f;
	material_bike.diffuse_color[2] = 0.900000f;
	material_bike.diffuse_color[3] = 1.0f;

	material_bike.specular_color[0] = 1.0f;
	material_bike.specular_color[1] = 1.0f;
	material_bike.specular_color[2] = 1.0f;
	material_bike.specular_color[3] = 1.0f;

	material_bike.specular_exponent = 522.334717f;

	material_bike.emissive_color[0] = 0.000000f;
	material_bike.emissive_color[1] = 0.000000f;
	material_bike.emissive_color[2] = 0.000000f;
	material_bike.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_BIKE]);

	My_glTexImage2D_from_file("Data/static_objects/Bike.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_bike(void) {
	set_material(&material_bike);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_BIKE]);

	glFrontFace(GL_CW);

	glBindVertexArray(bike_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bike_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

}

// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;

Material_Parameters material_bus;

void prepare_bus(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry_vnt(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_bus.ambient_color[0] = 0.4f;
	material_bus.ambient_color[1] = 0.4f;
	material_bus.ambient_color[2] = 0.4f;
	material_bus.ambient_color[3] = 1.0f;

	material_bus.diffuse_color[0] = 0.96862f;
	material_bus.diffuse_color[1] = 0.90980f;
	material_bus.diffuse_color[2] = 0.79607f;
	material_bus.diffuse_color[3] = 1.0f;

	material_bus.specular_color[0] = 0.5f;
	material_bus.specular_color[1] = 0.5f;
	material_bus.specular_color[2] = 0.5f;
	material_bus.specular_color[3] = 1.0f;

	material_bus.specular_exponent = 50.334717f;

	material_bus.emissive_color[0] = 0.000000f;
	material_bus.emissive_color[1] = 0.000000f;
	material_bus.emissive_color[2] = 0.000000f;
	material_bus.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_BUS]);

	My_glTexImage2D_from_file("Data/static_objects/Bus01_A.tga");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_bus(void) {
	set_material(&material_bus);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_BUS]);

	glFrontFace(GL_CW);

	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// godzilla object
GLuint godzilla_VBO, godzilla_VAO;
int godzilla_n_triangles;
GLfloat* godzilla_vertices;

Material_Parameters material_godzilla;

void prepare_godzilla(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, godzilla_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/godzilla_vnt.geom");
	godzilla_n_triangles = read_geometry_vnt(&godzilla_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	godzilla_n_total_triangles += godzilla_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &godzilla_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glBufferData(GL_ARRAY_BUFFER, godzilla_n_total_triangles * 3 * n_bytes_per_vertex, godzilla_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(godzilla_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &godzilla_VAO);
	glBindVertexArray(godzilla_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, godzilla_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_godzilla.ambient_color[0] = 0.5f;
	material_godzilla.ambient_color[1] = 0.5f;
	material_godzilla.ambient_color[2] = 0.5f;
	material_godzilla.ambient_color[3] = 1.0f;

	material_godzilla.diffuse_color[0] = 0.4f;
	material_godzilla.diffuse_color[1] = 0.4f;
	material_godzilla.diffuse_color[2] = 0.2f;
	material_godzilla.diffuse_color[3] = 1.0f;

	material_godzilla.specular_color[0] = 0.4f;
	material_godzilla.specular_color[1] = 0.4f;
	material_godzilla.specular_color[2] = 0.2f;
	material_godzilla.specular_color[3] = 1.0f;

	material_godzilla.specular_exponent = 5.334717f;

	material_godzilla.emissive_color[0] = 0.000000f;
	material_godzilla.emissive_color[1] = 0.000000f;
	material_godzilla.emissive_color[2] = 0.000000f;
	material_godzilla.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_GODZILLA]);

	My_glTexImage2D_from_file("Data/static_objects/Godzilla_D.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void draw_godzilla(void) {
	set_material(&material_godzilla);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_GODZILLA]);

	glFrontFace(GL_CW);

	glBindVertexArray(godzilla_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * godzilla_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// ironman object
GLuint ironman_VBO, ironman_VAO;
int ironman_n_triangles;
GLfloat* ironman_vertices;

Material_Parameters material_ironman;

void prepare_ironman(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, ironman_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ironman_vnt.geom");
	ironman_n_triangles = read_geometry_vnt(&ironman_vertices, n_bytes_per_triangle, filename);
	ironman_n_triangles = read_geometry_vnt(&ironman_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ironman_n_total_triangles += ironman_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &ironman_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glBufferData(GL_ARRAY_BUFFER, ironman_n_total_triangles * 3 * n_bytes_per_vertex, ironman_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ironman_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ironman_VAO);
	glBindVertexArray(ironman_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_ironman.ambient_color[0] = 0.4f;
	material_ironman.ambient_color[1] = 0.4f;
	material_ironman.ambient_color[2] = 0.4f;
	material_ironman.ambient_color[3] = 1.0f;

	material_ironman.diffuse_color[0] = 0.900000f;
	material_ironman.diffuse_color[1] = 0.200000f;
	material_ironman.diffuse_color[2] = 0.200000f;
	material_ironman.diffuse_color[3] = 1.0f;

	material_ironman.specular_color[0] = 1.0f;
	material_ironman.specular_color[1] = 1.0f;
	material_ironman.specular_color[2] = 1.0f;
	material_ironman.specular_color[3] = 1.0f;

	material_ironman.specular_exponent = 52.334717f;

	material_ironman.emissive_color[0] = 0.000000f;
	material_ironman.emissive_color[1] = 0.000000f;
	material_ironman.emissive_color[2] = 0.000000f;
	material_ironman.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_IRONMAN]);

	My_glTexImage2D_from_file("Data/static_objects/Ironman_Body.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_ironman(void) {
	set_material(&material_ironman);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_IRONMAN]);

	glFrontFace(GL_CW);

	glBindVertexArray(ironman_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ironman_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// tank object
GLuint tank_VBO, tank_VAO;
int tank_n_triangles;
GLfloat* tank_vertices;

Material_Parameters material_tank;

void prepare_tank(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, tank_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/tank_vnt.geom");
	tank_n_triangles = read_geometry_vnt(&tank_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	tank_n_total_triangles += tank_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &tank_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glBufferData(GL_ARRAY_BUFFER, tank_n_total_triangles * 3 * n_bytes_per_vertex, tank_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(tank_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &tank_VAO);
	glBindVertexArray(tank_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_tank.ambient_color[0] = 0.0f;
	material_tank.ambient_color[1] = 0.0f;
	material_tank.ambient_color[2] = 0.0f;
	material_tank.ambient_color[3] = 1.0f;

	material_tank.diffuse_color[0] = 0.200000f;
	material_tank.diffuse_color[1] = 0.980000f;
	material_tank.diffuse_color[2] = 0.500000f;
	material_tank.diffuse_color[3] = 1.0f;

	material_tank.specular_color[0] = 1.0f;
	material_tank.specular_color[1] = 1.0f;
	material_tank.specular_color[2] = 1.0f;
	material_tank.specular_color[3] = 1.0f;

	material_tank.specular_exponent = 52.334717f;

	material_tank.emissive_color[0] = 0.000000f;
	material_tank.emissive_color[1] = 0.000000f;
	material_tank.emissive_color[2] = 0.000000f;
	material_tank.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_TANK]);

	My_glTexImage2D_from_file("Data/static_objects/tank.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_tank(void) {
	set_material(&material_tank);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_TANK]);

	glFrontFace(GL_CW);

	glBindVertexArray(tank_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tank_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// nathan object
#define N_NATHAN_FRAMES 69
GLuint nathan_VBO, nathan_VAO;
int nathan_n_triangles[N_NATHAN_FRAMES];
int nathan_vertex_offset[N_NATHAN_FRAMES];
GLfloat* nathan_vertices[N_NATHAN_FRAMES];

Material_Parameters material_nathan;

void prepare_nathan(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, nathan_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_NATHAN_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/nathan/rp_nathan_animated_003_walking%d.geom", i);
		nathan_n_triangles[i] = read_geometry_vnt(&nathan_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		nathan_n_total_triangles += nathan_n_triangles[i];

		if (i == 0)
			nathan_vertex_offset[i] = 0;
		else
			nathan_vertex_offset[i] = nathan_vertex_offset[i - 1] + 3 * nathan_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &nathan_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, nathan_VBO);
	glBufferData(GL_ARRAY_BUFFER, nathan_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_NATHAN_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, nathan_vertex_offset[i] * n_bytes_per_vertex,
			nathan_n_triangles[i] * n_bytes_per_triangle, nathan_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_NATHAN_FRAMES; i++)
		free(nathan_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &nathan_VAO);
	glBindVertexArray(nathan_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, nathan_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_nathan.ambient_color[0] = 0.0f;
	material_nathan.ambient_color[1] = 0.0f;
	material_nathan.ambient_color[2] = 0.0f;
	material_nathan.ambient_color[3] = 1.0f;

	material_nathan.diffuse_color[0] = 0.9922f;
	material_nathan.diffuse_color[1] = 0.9922f;
	material_nathan.diffuse_color[2] = 0.9922f;
	material_nathan.diffuse_color[3] = 1.0f;

	material_nathan.specular_color[0] = 0.1184f;
	material_nathan.specular_color[1] = 0.0665f;
	material_nathan.specular_color[2] = 0.0512f;
	material_nathan.specular_color[3] = 1.0f;

	material_nathan.specular_exponent = 50.334717f;

	material_nathan.emissive_color[0] = 0.000000f;
	material_nathan.emissive_color[1] = 0.000000f;
	material_nathan.emissive_color[2] = 0.000000f;
	material_nathan.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_NATHAN]);

	My_glTexImage2D_from_file("Data/dynamic_objects/nathan/rp_nathan_animated_003_dif.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_nathan(void) {
	set_material(&material_nathan);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_NATHAN]);

	glFrontFace(GL_CW);

	glBindVertexArray(nathan_VAO);
	glDrawArrays(GL_TRIANGLES, nathan_vertex_offset[cur_frame_nathan], 3 * nathan_n_triangles[cur_frame_nathan]);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// cat object
GLuint cat_VBO, cat_VAO;
int cat_n_triangles;
GLfloat* cat_vertices;

Material_Parameters material_cat;

void prepare_cat(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, cat_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/cat_vnt.geom");
	cat_n_triangles = read_geometry_vnt(&cat_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	cat_n_total_triangles += cat_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &cat_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, cat_VBO);
	glBufferData(GL_ARRAY_BUFFER, cat_n_total_triangles * 3 * n_bytes_per_vertex, cat_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(cat_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &cat_VAO);
	glBindVertexArray(cat_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cat_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_cat.ambient_color[0] = 0.5f;
	material_cat.ambient_color[1] = 0.5f;
	material_cat.ambient_color[2] = 0.5f;
	material_cat.ambient_color[3] = 1.0f;

	material_cat.diffuse_color[0] = 0.7f;
	material_cat.diffuse_color[1] = 0.5f;
	material_cat.diffuse_color[2] = 0.2f;
	material_cat.diffuse_color[3] = 1.0f;

	material_cat.specular_color[0] = 0.4f;
	material_cat.specular_color[1] = 0.4f;
	material_cat.specular_color[2] = 0.2f;
	material_cat.specular_color[3] = 1.0f;

	material_cat.specular_exponent = 5.334717f;

	material_cat.emissive_color[0] = 0.000000f;
	material_cat.emissive_color[1] = 0.000000f;
	material_cat.emissive_color[2] = 0.000000f;
	material_cat.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_CAT]);

	My_glTexImage2D_from_file("Data/static_objects/cat_diff.tga");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_cat(void) {
	set_material(&material_cat);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_CAT]);

	glFrontFace(GL_CW);

	glBindVertexArray(cat_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * cat_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// ogre object
GLuint ogre_VBO, ogre_VAO;
int ogre_n_triangles;
GLfloat* ogre_vertices;

Material_Parameters material_ogre;

void prepare_ogre(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, ogre_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ogre_vnt.geom");
	ogre_n_triangles = read_geometry_vnt(&ogre_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ogre_n_total_triangles += ogre_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &ogre_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ogre_VBO);
	glBufferData(GL_ARRAY_BUFFER, ogre_n_total_triangles * 3 * n_bytes_per_vertex, ogre_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ogre_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ogre_VAO);
	glBindVertexArray(ogre_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ogre_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_ogre.ambient_color[0] = 0.5f;
	material_ogre.ambient_color[1] = 0.5f;
	material_ogre.ambient_color[2] = 0.5f;
	material_ogre.ambient_color[3] = 1.0f;

	material_ogre.diffuse_color[0] = 0.012f;
	material_ogre.diffuse_color[1] = 0.7f;
	material_ogre.diffuse_color[2] = 0.57f;
	material_ogre.diffuse_color[3] = 1.0f;

	material_ogre.specular_color[0] = 0.4f;
	material_ogre.specular_color[1] = 0.4f;
	material_ogre.specular_color[2] = 0.2f;
	material_ogre.specular_color[3] = 1.0f;

	material_ogre.specular_exponent = 5.334717f;

	material_ogre.emissive_color[0] = 0.000000f;
	material_ogre.emissive_color[1] = 0.000000f;
	material_ogre.emissive_color[2] = 0.000000f;
	material_ogre.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_OGRE]);

	My_glTexImage2D_from_file("Data/static_objects/ogre_diffuse.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_ogre(void) {
	set_material(&material_ogre);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_OGRE]);

	glFrontFace(GL_CW);

	glBindVertexArray(ogre_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ogre_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// ant object
GLuint ant_VBO, ant_VAO;
int ant_n_triangles;
GLfloat* ant_vertices;

Material_Parameters material_ant;

void prepare_ant(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, ant_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ant_vnt.geom");
	ant_n_triangles = read_geometry_vnt(&ant_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ant_n_total_triangles += ant_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &ant_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ant_VBO);
	glBufferData(GL_ARRAY_BUFFER, ant_n_total_triangles * 3 * n_bytes_per_vertex, ant_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ant_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ant_VAO);
	glBindVertexArray(ant_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ant_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// need to change values
	material_ant.ambient_color[0] = 0.5f;
	material_ant.ambient_color[1] = 0.5f;
	material_ant.ambient_color[2] = 0.5f;
	material_ant.ambient_color[3] = 1.0f;

	material_ant.diffuse_color[0] = 0.7f;
	material_ant.diffuse_color[1] = 0.5f;
	material_ant.diffuse_color[2] = 0.2f;
	material_ant.diffuse_color[3] = 1.0f;

	material_ant.specular_color[0] = 0.4f;
	material_ant.specular_color[1] = 0.4f;
	material_ant.specular_color[2] = 0.2f;
	material_ant.specular_color[3] = 1.0f;

	material_ant.specular_exponent = 5.334717f;

	material_ant.emissive_color[0] = 0.000000f;
	material_ant.emissive_color[1] = 0.000000f;
	material_ant.emissive_color[2] = 0.000000f;
	material_ant.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ANT]);

	My_glTexImage2D_from_file("Data/static_objects/antTexture.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_ant(void) {
	set_material(&material_ant);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_ANT]);

	glFrontFace(GL_CW);

	glBindVertexArray(ant_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ant_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// city object
GLuint city_VBO, city_VAO;
int city_n_triangles;
GLfloat* city_vertices;

int city_n_material_indices;
int* city_material_indices;

int city_n_materials;
char** city_diffuse_texture;
GLuint* city_texture_names;

Material_Parameters* material_city;

void prepare_city(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, city_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	bool bOnce = false;
	sprintf(filename, "Data/static_objects/BlenderCity_vntm.geom");
	city_n_triangles = read_geometry_vntm(&city_vertices, n_bytes_per_triangle,
		&city_n_material_indices,
		&city_material_indices,
		&city_n_materials,
		&city_diffuse_texture,
		&material_city,
		&bOnce,
		filename);

	// assume all geometry files are effective
	city_n_total_triangles += city_n_triangles;
	
	// initialize vertex buffer object
	glGenBuffers(1, &city_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, city_VBO);
	glBufferData(GL_ARRAY_BUFFER, city_n_total_triangles * n_bytes_per_triangle, city_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(city_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &city_VAO);
	glBindVertexArray(city_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, city_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	city_texture_names = (GLuint*)malloc(sizeof(GLuint) * city_n_materials);
	glGenTextures(city_n_materials, city_texture_names);

	for (int i = 0; i < city_n_materials; i++) {
		if (strcmp("", city_diffuse_texture[i])) {
			glBindTexture(GL_TEXTURE_2D, city_texture_names[i]);

			sprintf(filename, "Data/static_objects/%s", city_diffuse_texture[i]);
			My_glTexImage2D_from_file(filename);

			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void draw_city(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(city_VAO);

	for (int i = 0; i < city_n_material_indices - 1; i++) {
		int cur_material_id = city_material_indices[2 * i];
		set_material(&material_city[cur_material_id]);
		bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, city_texture_names[cur_material_id]);

		int cur_vertex_offset = city_material_indices[2 * i + 1];
		int n_vertices = city_material_indices[2 * (i + 1) + 1] - city_material_indices[2 * i + 1];
		glDrawArrays(GL_TRIANGLES, cur_vertex_offset, n_vertices);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);
}

// helicopter object
GLuint helicopter_VBO, helicopter_VAO;
int helicopter_n_triangles;
GLfloat* helicopter_vertices;

int helicopter_n_material_indices;
int* helicopter_material_indices;

int helicopter_n_materials;
char** helicopter_diffuse_texture;
GLuint* helicopter_texture_names;

Material_Parameters* material_helicopter;

void prepare_helicopter(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, helicopter_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	bool bOnce = false;

	sprintf(filename, "Data/static_objects/helicopter_vntm.geom");
	helicopter_n_triangles = read_geometry_vntm(&helicopter_vertices, n_bytes_per_triangle,
		&helicopter_n_material_indices,
		&helicopter_material_indices,
		&helicopter_n_materials,
		&helicopter_diffuse_texture,
		&material_helicopter,
		&bOnce,
		filename);

	// assume all geometry files are effective
	helicopter_n_total_triangles += helicopter_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &helicopter_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, helicopter_VBO);
	glBufferData(GL_ARRAY_BUFFER, helicopter_n_total_triangles * n_bytes_per_triangle, helicopter_vertices, GL_STATIC_DRAW);

	free(helicopter_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &helicopter_VAO);
	glBindVertexArray(helicopter_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, helicopter_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	helicopter_texture_names = (GLuint*)malloc(sizeof(GLuint) * helicopter_n_materials);
	glGenTextures(helicopter_n_materials, helicopter_texture_names);

	for (int i = 0; i < helicopter_n_materials; i++) {
		if (strcmp("", helicopter_diffuse_texture[i])) {
			glBindTexture(GL_TEXTURE_2D, helicopter_texture_names[i]);

			sprintf(filename, "Data/static_objects/%s", helicopter_diffuse_texture[i]);
			My_glTexImage2D_from_file(filename);

			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void draw_helicopter(void) {
	glFrontFace(GL_CW);

	glBindVertexArray(helicopter_VAO);

	for (int i = 0; i < helicopter_n_material_indices - 1; i++) {
		int cur_material_id = helicopter_material_indices[2 * i];
		set_material(&material_helicopter[cur_material_id]);
		bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, helicopter_texture_names[cur_material_id]);
		
		int cur_vertex_offset = helicopter_material_indices[2 * i + 1];
		int n_vertices = helicopter_material_indices[2 * (i + 1) + 1] - helicopter_material_indices[2 * i + 1];
		glDrawArrays(GL_TRIANGLES, cur_vertex_offset, n_vertices);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);
}

// tower object
GLuint tower_VBO, tower_VAO;
int tower_n_triangles;
GLfloat* tower_vertices;

Material_Parameters material_tower;

void prepare_tower(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, tower_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/woodTower_vnt.geom");
	tower_n_triangles = read_geometry_vnt(&tower_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	tower_n_total_triangles += tower_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &tower_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tower_VBO);
	glBufferData(GL_ARRAY_BUFFER, tower_n_total_triangles * 3 * n_bytes_per_vertex, tower_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(tower_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &tower_VAO);
	glBindVertexArray(tower_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tower_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_tower.ambient_color[0] = 1.0f;
	material_tower.ambient_color[1] = 1.0f;
	material_tower.ambient_color[2] = 1.0f;
	material_tower.ambient_color[3] = 1.0f;

	material_tower.diffuse_color[0] = 0.4f;
	material_tower.diffuse_color[1] = 0.4f;
	material_tower.diffuse_color[2] = 0.4f;
	material_tower.diffuse_color[3] = 1.0f;

	material_tower.specular_color[0] = 0.0f;
	material_tower.specular_color[1] = 0.0f;
	material_tower.specular_color[2] = 0.0f;
	material_tower.specular_color[3] = 1.0f;

	material_tower.specular_exponent = 0.0f;

	material_tower.emissive_color[0] = 0.000000f;
	material_tower.emissive_color[1] = 0.000000f;
	material_tower.emissive_color[2] = 0.000000f;
	material_tower.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_TOWER]);

	My_glTexImage2D_from_file("Data/static_objects/Wood_Tower_Col.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_tower(void) {
	set_material(&material_tower);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_TOWER]);

	glFrontFace(GL_CW);

	glBindVertexArray(tower_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tower_n_triangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}
/********************************* END: objects *********************************/

// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &rectangle_VAO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteVertexArrays(1, &ben_VAO);
	glDeleteBuffers(1, &ben_VBO);

	for (int i = 0; i < N_BEN_FRAMES; i++) 
		free(ben_material_indices[i]);

	for (int i = 0; i < ben_n_materials; i++)
		free(ben_diffuse_texture[i]);
	
	free(ben_diffuse_texture);
	free(material_ben);
	glDeleteTextures(ben_n_materials, ben_texture_names);

	glDeleteVertexArrays(1, &wolf_VAO);
	glDeleteBuffers(1, &wolf_VBO);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

	glDeleteVertexArrays(1, &dragon_VAO);
	glDeleteBuffers(1, &dragon_VBO);

	glDeleteVertexArrays(1, &optimus_VAO);
	glDeleteBuffers(1, &optimus_VBO);

	glDeleteVertexArrays(1, &cow_VAO);
	glDeleteBuffers(1, &cow_VBO);

	glDeleteVertexArrays(1, &bus_VAO);
	glDeleteBuffers(1, &bus_VBO);

	glDeleteVertexArrays(1, &bike_VAO);
	glDeleteBuffers(1, &bike_VBO);

	glDeleteVertexArrays(1, &godzilla_VAO);
	glDeleteBuffers(1, &godzilla_VBO);

	glDeleteVertexArrays(1, &ironman_VAO);
	glDeleteBuffers(1, &ironman_VBO);

	glDeleteVertexArrays(1, &tank_VAO);
	glDeleteBuffers(1, &tank_VBO);

	glDeleteVertexArrays(1, &nathan_VAO);
	glDeleteBuffers(1, &nathan_VBO);

	glDeleteVertexArrays(1, &ogre_VAO);
	glDeleteBuffers(1, &ogre_VBO);

	glDeleteVertexArrays(1, &cat_VAO);
	glDeleteBuffers(1, &cat_VBO);

	glDeleteVertexArrays(1, &ant_VAO);
	glDeleteBuffers(1, &ant_VBO);

	glDeleteVertexArrays(1, &city_VAO);
	glDeleteBuffers(1, &city_VBO);

	free(city_material_indices);
	for (int i = 0; i < city_n_materials; i++)
		free(city_diffuse_texture[i]);
	
	free(city_diffuse_texture);
	free(material_city);
	glDeleteTextures(city_n_materials, city_texture_names);

	glDeleteVertexArrays(1, &helicopter_VAO);
	glDeleteBuffers(1, &helicopter_VBO);

	free(helicopter_material_indices);
	for (int i = 0; i < helicopter_n_materials; i++)
		free(helicopter_diffuse_texture[i]);

	free(helicopter_diffuse_texture);
	free(material_helicopter);
	glDeleteTextures(helicopter_n_materials, helicopter_texture_names);

	glDeleteVertexArrays(1, &tower_VAO);
	glDeleteBuffers(1, &tower_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_diffuse_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_flags(void) {
	flag_animation = 1;
	flag_polygon_fill = 1;
	flag_texture_mapping = true;
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(500.0f, 300.0f, 500.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ViewMatrix = glm::lookAt(4.0f / 6.0f * glm::vec3(500.0f, 600.0f, 500.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	initialize_lights_and_material();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
	light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
	light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

	light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
	light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
	light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
	light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
	light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 8.0f;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_tiger();
	prepare_ben();
	prepare_wolf();
	prepare_spider();
	prepare_dragon();
	prepare_optimus();
	prepare_cow();
	prepare_bus();
	prepare_bike();
	prepare_godzilla();
	prepare_ironman();
	prepare_tank();
	prepare_nathan();
	prepare_cat();
	prepare_ogre();
	prepare_ant();
	prepare_city();
	prepare_helicopter();
	prepare_tower();
	set_up_scene_lights();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects (2023)";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'a', 't', 'f', 'c', 'd', 'y', 'u', 'i', 'o', 'n', 'ESC'" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}