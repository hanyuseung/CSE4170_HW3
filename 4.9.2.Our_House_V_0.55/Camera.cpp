#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

typedef struct vp {
	int x, y, w, h;
}vp;


vp CC0; vp CC1; vp CC2;
void init_CC(int win_width, int win_height) {
	CC0.x = 0; CC0.y = 0; CC0.w = win_width - 800; CC0.h = win_height - 600;
	CC1.x = 0; CC1.y = win_height - 200, CC1.w = win_width - 700; CC1.h = win_height - 600;
	CC2.x = 0; CC2.y = win_height / 2 - 100; CC2.w = win_width - 900; CC2.h = win_height - 600;
}



void Orthographic_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	glm::mat3 R33_t;
	glm::mat4 T;
	init_CC(win_width, win_height);


	switch (camera_id) {
	case CAMERA_ORTHO_X:
		flag_valid = false;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(800.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]); // 정면과 반대 방향
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.bottom = -10.0f;
		cam_proj.params.ortho.top = 60.0f;
		cam_proj.params.ortho.left = -10.0f;
		cam_proj.params.ortho.right = 170.0f;
		cam_proj.params.ortho.f = 50000.0f;
		cam_proj.params.ortho.n = 1.0f;

		/*ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);*/
		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom,
			cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = CC0.x; view_port.y = CC0.y; view_port.w = CC0.w; view_port.h = CC0.h;

		break;
	case CAMERA_ORTHO_Y:
		flag_valid = false;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, 800.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.bottom = -10.0f;
		cam_proj.params.ortho.top = 60.0f;
		cam_proj.params.ortho.left = -260.0f;
		cam_proj.params.ortho.right = 20.0f;
		cam_proj.params.ortho.f = 50000.0f;
		cam_proj.params.ortho.n = 1.0f;

		/*ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);*/
		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom,
			cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = CC1.x; view_port.y = CC1.y; view_port.w = CC1.w; view_port.h = CC1.h;
		break;
	case CAMERA_ORTHO_Z:
		flag_valid = false;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(120.0f, 80.0f, 100.0f), glm::vec3(120.0f, 80.0f, 1.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.bottom = -90.0f;
		cam_proj.params.ortho.top = 90.0f;
		cam_proj.params.ortho.left = -130.0f;
		cam_proj.params.ortho.right = 130.0f;
		cam_proj.params.ortho.f = 50000.0f;
		cam_proj.params.ortho.n = 1.0f;

		/*ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);*/
		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom,
			cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = CC2.x; view_port.y = CC2.y; view_port.w = CC2.w; view_port.h = CC2.h;
		break;
	}
}

void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	glm::mat3 R33_t;
	glm::mat4 T;
	init_CC(win_width, win_height);
	


 
	switch (camera_id) {
	case CAMERA_MAIN:
		flag_valid = true;
		flag_move = true; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(-600.0f, -600.0f, 400.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;  
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 300; view_port.y = win_height/2- 250; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;

	case CAMERA_SIDE:
		flag_valid = false;
		flag_move = false; 

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(800.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
		cam_proj.params.ortho.bottom = -150.0f;
		cam_proj.params.ortho.top = 150.0f;
		cam_proj.params.ortho.left = -200.0f;
		cam_proj.params.ortho.right = 200.0f;
		cam_proj.params.ortho.f = 50000.0f;
		cam_proj.params.ortho.n = 1.0f;
		
		/*ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);*/
		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right, cam_proj.params.ortho.bottom,
			cam_proj.params.ortho.top, cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		view_port.x = -(win_width/2) + 200; view_port.y =win_height/2 - 100; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;

	case CAMERA_SIDE_FRONT:
		flag_valid = false;
		flag_move = false; 

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, 600.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 200; view_port.y = 200; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;

	case CAMERA_TOP:
		flag_valid = false;
		flag_move = false; 

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 800.0f), glm::vec3(50.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 200; view_port.y = 200; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;

	case CAMERA_CC_0:
		flag_valid = true;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(185.0f, 130.0f, 25.0f), glm::vec3(240.0f, 70.0, 10.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 80.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = CC0.x; view_port.y = CC0.y; view_port.w = CC0.w; view_port.h = CC0.h;
		break;

	case CAMERA_CC_1:
		flag_valid = true;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(70.0f, 120.0f, 40.0f), glm::vec3(165.0f, 55.0f,20.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 70.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = CC1.x; view_port.y = CC1.y; view_port.w = CC1.w; view_port.h = CC1.h;
		break;

	case CAMERA_CC_2:
		flag_valid = true;
		flag_move = false; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(15.0f, 15.0f, 40.0f), glm::vec3(150.0f, 100.0f, 10.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 60.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = CC2.x; view_port.y = CC2.y; view_port.w = CC2.w; view_port.h = CC2.h;
		break;

	case CAMERA_DYNAMIC:
		flag_valid = false;
		flag_move = true;

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(225.0f, 15.0f, 25.0f), glm::vec3(70.0f, 12.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 45.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 300; view_port.y = 0; view_port.w = win_width - 300; view_port.h = win_height;
		break;
	}
	
}

