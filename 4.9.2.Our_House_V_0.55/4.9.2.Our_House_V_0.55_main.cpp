#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

using namespace glm;

Scene scene;

bool Camera_rotate_flag = false;
const int FPS = 60;
const float DT = 1.0f/FPS;

typedef struct Flag {
	int Camera_xyz = { 0 };
	bool Camera_rotate = { false };
	bool Camera_gofront = { false };
	bool Camera_goback = { false };
}Flag;

typedef struct Mouse {
	vec2 point;
	float zoomfactor = { 1.0f };
}Mouse;

Mouse mouse;

Flag flag;

void update();
void updateCamRotate(float dt);
void zoomInOut(float z);
void mouseWheel(int wheel, int direction, int x, int y);

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().flag_valid == false) continue;
		if (Camera_rotate_flag) {
			camera->get().ViewMatrix = rotate(camera->get().ViewMatrix,  100*DT * TO_RADIAN , vec3(0.0f, 0.0f, 1.0f));
		}
		if (flag.Camera_xyz == CAMERA_SIDE) {
			
		}
		glViewport(camera->get().view_port.x, camera->get().view_port.y,
			camera->get().view_port.w, camera->get().view_port.h);
		scene.ViewMatrix = camera->get().ViewMatrix;
		scene.ProjectionMatrix = camera->get().ProjectionMatrix;

		scene.draw_world();
	}
	

	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'x':
		// Change vm to x axis
		flag.Camera_xyz = CAMERA_SIDE;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_SIDE) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to side front view.\n");
		glutPostRedisplay();
		break;
	case 'y':
		// Change vm to y axis
		flag.Camera_xyz = CAMERA_SIDE_FRONT;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_SIDE_FRONT) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to side front view.\n");
		glutPostRedisplay();
		break;
	case 'z':
		// Change vm to z axis
		flag.Camera_xyz = CAMERA_TOP;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_TOP) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to Top view.\n");
		glutPostRedisplay();
		break;
	case ' ':
		// Change vm to main axis
		flag.Camera_xyz = CAMERA_MAIN;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_MAIN) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to main view.\n");
		glutPostRedisplay();
		break;
	case 'r':
		if (Camera_rotate_flag) {
			Camera_rotate_flag = false;
		}
		else {
			Camera_rotate_flag = true;
		}
		break;
	case 'w':
		break;
	case 's':
		break;
	}

}

void keyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		break;
	case 's':
		break;
	}
		
}

void mouseWheel(int wheel, int direction, int x, int y) {
	float zoomf = 1.0f;
	if (direction < 0) {
		// 휠을 위로 굴렸을 때 (줌 인)
		//mouse.zoomfactor = 1.1f;
		printf("zoomin\n");
		zoomf = 1.1f;
	}
	else {
		// 휠을 아래로 굴렸을 때 (줌 아웃)
		//mouse.zoomfactor = 1.0f/1.1f;
		zoomf = 0.9f;
	}
	// 필요하다면 뷰투영 행렬을 갱신하거나, 카메라 속성 업데이트
	// 예: UpdateProjectionMatrix(g_zoom);
	// 화면을 다시 그리도록 요청
	zoomInOut(zoomf);
	glutPostRedisplay();
	mouse.zoomfactor = 1.0f;
}

void zoomInOut(float zoomf){
	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().flag_valid) {
			if ((camera->get().cam_proj.params.pers.fovy) / TO_RADIAN > 120.0f) {
				camera->get().cam_proj.params.pers.fovy = 119.0f*TO_RADIAN;
				return;
			}
			else if((camera->get().cam_proj.params.pers.fovy) / TO_RADIAN < 10.0f) {
				camera->get().cam_proj.params.pers.fovy = 11.0f*TO_RADIAN;
				return;
			}
			(camera->get().cam_proj.params.pers.fovy) *= zoomf;
			camera->get().ProjectionMatrix = perspective(camera->get().cam_proj.params.pers.fovy, camera->get().cam_proj.params.pers.aspect,
				camera->get().cam_proj.params.pers.n, camera->get().cam_proj.params.pers.f);
		}
	}
}



void update()
{

}

void updateCamRotate(float dt) 
{
	
}

void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}

void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer_scene, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutReshapeFunc(reshape);
 	glutTimerFunc(100, timer_scene, 0);
	glutMouseWheelFunc(mouseWheel);
//	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
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

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: fill it yourself!" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
