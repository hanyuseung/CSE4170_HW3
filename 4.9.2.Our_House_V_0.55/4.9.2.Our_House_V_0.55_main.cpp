#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

using namespace glm;

Scene scene;

#define CAMERA_X 0;
#define CAMERA_Y 1;
#define CAMERA_Z 2;

const int FPS = 60;
const float DT = 1.0f/FPS;

typedef struct Flag {
	int Camera_xyz = { 0 };
	bool camera_wasd[6] = { false, false, false, false, false, false};
	bool Camera_rotate = { false };
	bool Camera_move = { false };
	bool Camera_clear = { false };
	bool Camera_Ortho = { false };
	bool First = { false };
}Flag;

typedef struct Mouse {
	vec2 point;
	vec2 lastPoint;
	vec2 angle; // 좌우, 상하
	float zoomfactor = { 1.0f };
	bool leftPressed = { false }, rightPressed = { false };
	bool firstMouse = { true };
	vec3 camFront;
}Mouse;

Mouse mouse;

Flag flag;

void update();
void updateCamRotate(float dt);
void zoomInOut(float z);
void mouseWheel(int wheel, int direction, int x, int y);
void centerMouse() {
	int centerX = scene.window.width / 2;
	int centerY = scene.window.height / 2;
	glutWarpPointer(centerX, centerY);
	mouse.lastPoint = vec2(0, 0);
}



void display(void) {
	// To get shift key input
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().flag_valid == false) continue;
		//clear main camera
		if (flag.Camera_clear && camera-> get().camera_id == CAMERA_MAIN) { 
			camera->get().ViewMatrix = lookAt(vec3(-600.0f, -600.0f, 400.0f), vec3(125.0f, 80.0f, 25.0f), vec3(0.0f, 0.0f, 1.0f)); 
			flag.Camera_clear = false;
		}
		// cam rotate
		if (flag.Camera_rotate && camera->get().flag_move) {
			
			switch (flag.Camera_xyz) {
			case 0: 
				camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, 100 * DT * TO_RADIAN, vec3(1.0f, 0.0f, 0.0f));
				break;
			case 1:
				camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, 100 * DT * TO_RADIAN, vec3(0.0f, 1.0f, 0.0f));
				break;
			case 2:
				camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, 100 * DT * TO_RADIAN, vec3(0.0f, 0.0f, 1.0f));
				break;
			}
		}
		
		if (flag.Camera_move && camera->get().camera_id == CAMERA_MAIN) {
		
			mat4 oldVM = camera->get().ViewMatrix;
			mat4 R = rotate(mat4(1.0f), mouse.angle.y, vec3(1.0f, 0.0f, 0.0f));
			R = rotate(R, -mouse.angle.x, vec3(0.0f, 1.0f, 0.0f));
			R = transpose(R);
			R = R * oldVM;
			
			camera->get().ViewMatrix = R;
			
			vec3 new_uaxis = vec3(R[0][0], R[1][0], R[2][0]);
			//camera->get().cam_view.uaxis = new_uaxis;
			vec3 new_naxis = vec3(R[0][2], R[1][2], R[2][2]);
			vec3 new_vaxis = vec3(R[0][1], R[1][1], R[2][1]);
			
			vec3 front = normalize(-new_naxis);
			front.z = 0.0f;
			vec3 side = normalize(new_uaxis);
			side.z = 0.0f;
			vec3 updown = normalize(new_vaxis);
			updown.x = updown.y = 0.0f;

			for (int i = 0; i < 6; i++) {
				if (flag.camera_wasd[i]) {
					
					vec3 movement(0.0f);
					switch (i) {
					// 바라보고 있는 방향으로 움직이려면? n axis normalize해서 가중.
					case 0: // w
						movement = front * 100.0f * DT;
						break;
					// 바로보고 있는 방향 옆으로 움직이기: u axis normalize 해서 가중.
					case 3: // d
						movement = side * 100.0f * DT;
						break;
					case 2: // s
						movement = -front * 100.0f * DT;
						break;
					case 1: // a
						movement = -side * 100.0f * DT;
						break;
					case 4: // ' '
						movement = updown * 100.0f * DT;
						break;
					case 5: // z
						movement = -updown * 100.0f * DT;
						break;
					}
					mat4 newmat = translate(mat4(1.0f), -movement);
					camera->get().ViewMatrix = camera->get().ViewMatrix * newmat;
				}
			}
			
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
	case '2':
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
		if (flag.Camera_rotate) {
			flag.Camera_xyz = CAMERA_X;
			break;
		}

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

		if (flag.Camera_rotate) {
			flag.Camera_xyz = CAMERA_Y;
			break;
		}

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
		if (flag.Camera_move) {
			flag.camera_wasd[5] = true;
			break;
		}


		if (flag.Camera_rotate) {
			flag.Camera_xyz = CAMERA_Z;
			break;
		}

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
		if (flag.Camera_move) {
			flag.camera_wasd[4] = true;
			break;
		}
		// Change vm to main axis
		flag.Camera_xyz = CAMERA_MAIN;
		flag.Camera_clear = true;
		flag.Camera_Ortho = false;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_MAIN || camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2 ) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to main view & clear.\n");

		glutPostRedisplay();
		break;
	case 'r':
		if (flag.Camera_rotate) {
			flag.Camera_rotate = false;
		}
		else {
			flag.Camera_rotate = true;
		}
		glutPostRedisplay();
		break;
	case 'w':
		flag.camera_wasd[0] = true; 
		break;
	case 'a':
		flag.camera_wasd[1] = true;
		break;
	case 's':
		flag.camera_wasd[2] = true; 
		break;
	case 'd' :
		flag.camera_wasd[3] = true;
		break;
	case 'm':
		// Move mode 
		flag.Camera_move = 1 - flag.Camera_move;
		break;
	case '1':
		// Show orthogonal Cam on CCTV window.
		if(!flag.Camera_Ortho){
			for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
				if (camera->get().camera_id == CAMERA_ORTHO_X || camera->get().camera_id == CAMERA_ORTHO_Y || camera->get().camera_id == CAMERA_ORTHO_Z) {
					camera->get().flag_valid = true;
				}
				else if (camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) { 
					camera->get().flag_valid = false;
				}
			}
			flag.Camera_Ortho = true;
		}
		else {
			for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
				if (camera->get().camera_id == CAMERA_ORTHO_X || camera->get().camera_id == CAMERA_ORTHO_Y || camera->get().camera_id == CAMERA_ORTHO_Z) {
					camera->get().flag_valid = false;
				}
				else if (camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
					camera->get().flag_valid = true;
				}
			}
			flag.Camera_Ortho = false;
		}
		glutPostRedisplay();
		break;
	}




}

void keyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		flag.camera_wasd[0] = false;
		break;
	case 'a':
		flag.camera_wasd[1] = false;
		break;
	case 's':
		flag.camera_wasd[2] = false;
		break;
	case 'd':
		flag.camera_wasd[3] = false;
		break;
	case ' ':
		if (flag.Camera_move)
			flag.camera_wasd[4] = false;
		break;
	case 'z':
		if (flag.Camera_move) {
			flag.camera_wasd[5] = false;
			break;
		}
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

void passivemouse(int x, int y) {
	//printf("%d %d \n", x, y);

	mouse.point.x = -scene.window.width / 2 + x;
	mouse.point.y = scene.window.height / 2 - y;
	if (flag.Camera_move) {
		if (flag.First) {
			centerMouse();
			mouse.angle = vec2(-90.0f, 0.0f);
			mouse.lastPoint = vec2(0.0f,0.0f);
			flag.First = false;
			mouse.camFront = vec3(0.0f, -0.0f, -1.0f);
			return;
		}
		vec2 offset = mouse.point - mouse.lastPoint;
		mouse.lastPoint = vec2(0.0f,0.0f);
		offset *= 0.001f;
		//printf("%f %f\n", mouse.lastPoint.x, mouse.lastPoint.y);
		mouse.angle = offset;
		printf("%f %f\n", mouse.angle.x, mouse.angle.y);
		//if (mouse.angle.x > 89.0f) mouse.angle.x = 89.0f;
		//if (mouse.angle.y > 89.0f) mouse.angle.y = 89.0f;
		centerMouse();
	}


	
	
	//printf("mouse (%f %f)\n", mouse.point.x, mouse.point.y);
}

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse.leftPressed = true;
			mouse.rightPressed = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse.rightPressed = true;
			mouse.leftPressed = false;
		}
	}
}


void zoomInOut(float zoomf){
	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {

		if (camera->get().flag_valid && camera->get().flag_move) {
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
	glutPassiveMotionFunc(passivemouse);
	glutMouseFunc(mouseClick);
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
