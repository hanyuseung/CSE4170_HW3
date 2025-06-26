#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"
#include <random>

using namespace glm;

Scene scene;




#define WC_AXIS_LENGTH		60.0f

const int FPS = 60;
const float DT = 1.0f/FPS;

typedef struct Flag {
	int Camera_xyz = { 0 };
	bool camera_wasd[6] = { false, false, false, false, false, false};
	bool Camera_rotate = { false };
	bool Camera_Dynamic = { false };
	bool Camera_clear = { false };
	bool Camera_Ortho = { false };
	bool First = { false };
	bool Axis_Toggle = { false };
	int rotate_toggle = { 1 };

	bool Spider_done = { false };
	bool Ben_done = { false };
	bool Spider_start = { false };
	bool Ben_Die = { false };
	bool spider_die_start = { false };
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
void init_BenRoute();
void updateMMLight();




void centerMouse() {
	int centerX = scene.window.width / 2;
	int centerY = scene.window.height / 2;
	glutWarpPointer(centerX, centerY);
	mouse.lastPoint = vec2(0, 0);
}
void pprint_mat4(const char* string, glm::mat4 M);



#define LEFT 1
#define UP 2
#define DOWN 3
#define RIGHT 4 





typedef struct Route {
	glm::vec2 rt;
	int dir; // 1:left 2:up 3:down 4:right 
}Route;

std::vector <Route> R1;
std::vector <Route> R2;
std::vector <Route> R3;
std::vector <Route> R4;
std::vector <Route> SR4;

std::vector <Route> R2_1;
std::vector <Route> R3_1;
std::vector <Route> R4_1;

std::vector <Route> SR1;
std::vector <Route> SR2_1;
std::vector <Route> SR3_1;
std::vector <Route> SR4_1;

std::vector <Route> FullRoute;
std::vector <Route> SpiderRoute;


mat4 BenMM = mat4(1.0f);
mat4 SpiderMM = mat4(1.0f);
int b_cidx = 0;
int s_cidx = 0;
bool b_turndone = false;
bool s_turndone = false;

int b_size = 0;
int s_size = 0;


void Init_route() {
	scene.DynamicModelMatrixes.clear();
	BenMM = translate(mat4(1.0f), vec3(20.0f, 37.0f, 0.0f));
	SpiderMM = translate(mat4(1.0f), vec3(45.0f, 135.0f, 0.0f));
	scene.DynamicModelMatrixes.push_back(BenMM);
	scene.DynamicModelMatrixes.push_back(SpiderMM);

	

	Route route;
	R1.clear();
	R2.clear();
	R3.clear();
	R4.clear();
	R2_1.clear();
	R3_1.clear();
	R4_1.clear();
	SR4.clear();
	SR1.clear();
	SR2_1.clear();
	SR3_1.clear();
	SR4_1.clear();

	FullRoute.clear();
	SpiderRoute.clear();

	route.rt = glm::vec2(20.0f, 135.0f);
	route.dir = LEFT;
	SR1.push_back(route);
	route.rt = glm::vec2(20.0f, 80.0f);
	route.dir = DOWN;
	SR1.push_back(route);
	route.rt = glm::vec2(80.0f, 80.0f);
	route.dir = RIGHT;
	SR1.push_back(route);
	route.rt = glm::vec2(80.0f, 100.0f);
	route.dir = UP;
	SR1.push_back(route);
	route.rt = glm::vec2(125.0f, 100.0f);
	route.dir = RIGHT;
	SR1.push_back(route);
	

	

	// 기본

	route.rt = glm::vec2(20.0f, 20.0f);
	route.dir = DOWN;
	R1.push_back(route);
	route.rt = glm::vec2(80.0f, 20.0f);
	route.dir = RIGHT;
	R1.push_back(route);
	route.rt = glm::vec2(80.0f, 100.0f);
	route.dir = UP;
	R1.push_back(route);
	route.rt = glm::vec2(125.0f, 100.0f);
	route.dir = RIGHT;
	R1.push_back(route);

	route.rt = glm::vec2(165.0f, 100.0f);
	route.dir = RIGHT;
	R2.push_back(route);

	route.rt = glm::vec2(215.0f, 100.0f);
	route.dir = RIGHT;
	R3.push_back(route);

	route.rt = glm::vec2(215.0f, 65.0f);
	route.dir = DOWN;
	R4.push_back(route);
	route.rt = glm::vec2(185.0f, 65.0f);
	route.dir = LEFT;
	R4.push_back(route);
	route.rt = glm::vec2(185.0f, 45.0f);
	route.dir = DOWN;
	R4.push_back(route);


	route.rt = glm::vec2(215.0f, 65.0f);
	route.dir = DOWN;
	SR4.push_back(route);
	route.rt = glm::vec2(185.0f, 65.0f);
	route.dir = LEFT;
	SR4.push_back(route);
	route.rt = glm::vec2(185.0f, 60.0f);
	route.dir = DOWN;
	SR4.push_back(route);



	// 분기점 1

	route.rt = glm::vec2(125.0f, 65.0f);
	route.dir = DOWN;
	R2_1.push_back(route);
	route.rt = glm::vec2(110.0f, 65.0f);
	route.dir = LEFT;
	R2_1.push_back(route);
	route.rt = glm::vec2(110.0f, 20.0f);
	route.dir = DOWN;
	R2_1.push_back(route);
	route.rt = glm::vec2(135.0f, 20.0f);
	route.dir = RIGHT;
	R2_1.push_back(route);

	route.rt = glm::vec2(125.0f, 65.0f);
	route.dir = DOWN;
	SR2_1.push_back(route);
	route.rt = glm::vec2(110.0f, 65.0f);
	route.dir = LEFT;
	SR2_1.push_back(route);
	route.rt = glm::vec2(110.0f, 20.0f);
	route.dir = DOWN;
	SR2_1.push_back(route);
	route.rt = glm::vec2(115.0f, 20.0f);
	route.dir = RIGHT;
	SR2_1.push_back(route);
	

	//분기점 2
	route.rt = glm::vec2(165.0f, 147.5f);
	route.dir = UP;
	R3_1.push_back(route);
	route.rt = glm::vec2(130.0f, 147.5f);
	route.dir = LEFT;
	R3_1.push_back(route);

	route.rt = glm::vec2(165.0f, 147.5f);
	route.dir = UP;
	SR3_1.push_back(route);
	route.rt = glm::vec2(160.0f, 147.5f);
	route.dir = LEFT;
	SR3_1.push_back(route);


	// 분기점 3
	route.rt = glm::vec2(215.0f, 145.0f);
	route.dir = UP;
	R4_1.push_back(route);
	route.rt = glm::vec2(210.0f, 145.0f);
	route.dir = LEFT;
	R4_1.push_back(route);

	route.rt = glm::vec2(215.0f, 145.0f);
	route.dir = UP;
	SR4_1.push_back(route);
	route.rt = glm::vec2(215.0f, 145.0f);
	route.dir = LEFT;
	SR4_1.push_back(route);



	FullRoute.insert(FullRoute.end(), R1.begin(), R1.end());
	FullRoute.insert(FullRoute.end(), R2.begin(), R2.end());
	FullRoute.insert(FullRoute.end(), R3.begin(), R3.end());
	FullRoute.insert(FullRoute.end(), R4.begin(), R4.end());
	b_size = FullRoute.size();

	SpiderRoute.insert(SpiderRoute.end(), SR1.begin(), SR1.end());
	SpiderRoute.insert(SpiderRoute.end(), R2.begin(), R2.end());
	SpiderRoute.insert(SpiderRoute.end(), R3.begin(), R3.end());
	SpiderRoute.insert(SpiderRoute.end(), SR4.begin(), SR4.end());
	s_size = SpiderRoute.size();
}









void updateBenRoute() {

	
	mat4 RTM = mat4(1.0f);
	float angleRad = 0.0f;

	// 3. Ben이 도착함.
	if (b_size <= b_cidx) {
		
		// 3. spider 도착 전까지 대기
		if (!flag.Spider_done) {
			if (!b_turndone) {
				switch (FullRoute[b_cidx-1].dir) {
				case UP:
					angleRad = 2 * 90.0f * TO_RADIAN;
					RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
					break;
				case DOWN:

					break;
				case LEFT:
					angleRad = -1 * 90.0f * TO_RADIAN;
					RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
					break;
				case RIGHT:
					angleRad = 1 * 90.0f * TO_RADIAN;
					RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
					break;
				}
			}
			vec3 tmppos = vec3(FullRoute[b_cidx-1].rt.x, FullRoute[b_cidx-1].rt.y, 0.0f);
			mat4 BenMM = translate(mat4(1.0f), tmppos);
			scene.DynamicModelMatrixes[0] = BenMM * RTM;
			return;
		}

		// 고양이에게 가서 사망 - 미구현
		/*if (FullRoute[b_cidx - 1].rt == R4_1[R4_1.size() - 1].rt) {
			flag.Ben_Die = true;
		}*/
		

		// 4. spider die -> init!
		else {
			b_cidx = 0;
			init_BenRoute();
			flag.Ben_done = false;
		}
		return;
		
	}
	Route& route = FullRoute[b_cidx];


	// 1. Ben이 먼저 출발 후 spider에게 출발 신호 보내기
	if ((route.rt) == (R1[2].rt)) {
		flag.Spider_start = true;
	}
	
	
	if (!b_turndone) {
		switch (route.dir) {
		case UP:
			angleRad = 2 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		case DOWN:
			
			break;
		case LEFT:
			angleRad = -1 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		case RIGHT:
			angleRad = 1 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		}
	}

	vec2 curr_pos = vec2(BenMM[3][0], BenMM[3][1]);
	//printf("%f %f\n", curr_pos.x, curr_pos.y);
	vec2 target_pos = vec2(route.rt.x, route.rt.y); 
	vec2 difference = route.rt - curr_pos;
	float distance = length(curr_pos - target_pos);
	float step = 25.0f * DT;
	switch (route.dir) {
	case UP:
		if (difference.y < 0) { // 보정
			BenMM = translate(BenMM, vec3(difference.x, difference.y, 0.0f));
			b_cidx++;
			b_turndone = false;
		}
		else {
			BenMM = translate(BenMM, vec3(0.0f, step, 0.0f));
		}
		break;
	case DOWN:
		if (difference.y > 0) { // 보정
			BenMM = translate(BenMM, vec3(difference.x, difference.y, 0.0f));
			b_cidx++;
			b_turndone = false;
		}
		else {
			BenMM = translate(BenMM, vec3(0.0f, -step, 0.0f));
		}
		break;
	case LEFT:
		if (difference.x > 0) { // 보정
			BenMM = translate(BenMM, vec3(difference.x, difference.y, 0.0f));
			b_cidx++;
			b_turndone = false;
		}
		else {
			BenMM = translate(BenMM, vec3(-step, 0.0f, 0.0f));
		}
		break;
	case RIGHT:
		if (difference.x < 0) { // 보정
			BenMM = translate(BenMM, vec3(difference.x, difference.y, 0.0f));
			b_cidx++;
			b_turndone = false;
		}
		else {
			BenMM = translate(BenMM, vec3(step, 0.0f, 0.0f));
		}
		break;
	}
	
	scene.DynamicModelMatrixes[0] = BenMM*RTM;
}

void updateMMLight() {
	vec3 pos = vec3(BenMM[3]);
	mat3 R = mat3(BenMM);
	vec3 dir = normalize(R * vec3(0.0f, 0.0f, 1.0f));
	lightList[2].position[0] = pos.x;
	lightList[2].position[1] = pos.y;
	lightList[2].position[2] = pos.z;
	lightList[2].spot_direction[0] = dir.x;
	lightList[2].spot_direction[1] = dir.y;
	lightList[2].spot_direction[2] = dir.z;
}




int spiderDeathStart = -1;

void updateSpiderRoute() {

	// 2. Ben이 출발 보내기 전까지는 wait.
	if (!flag.Spider_start) {
		SpiderMM = translate(mat4(1.0f), vec3(45.0f, 135.0f, 0.0f));
		scene.DynamicModelMatrixes[1] = SpiderMM;
		return;
	}
	
	// 2. Ben이 출발 신호를 보냈다면


	// 4. Spider 도착 및 사망모션 시작
	mat4 RTM = mat4(1.0f);
	float angleRad = 0.0f;
	if (s_size <= s_cidx) {
		if (spiderDeathStart < 0) { // 사망 모션 시작 시각.
			spiderDeathStart = scene.time_stamp;
			flag.spider_die_start = true;
		}
		if (!b_turndone) {
			switch (SpiderRoute[s_cidx-1].dir) {
			case UP:
				angleRad = 2 * 90.0f * TO_RADIAN;
				RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
				break;
			case DOWN:
				break;
			case LEFT:
				angleRad = -1 * 90.0f * TO_RADIAN;
				RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
				break;
			case RIGHT:
				angleRad = 1 * 90.0f * TO_RADIAN;
				RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
				break;
			}
		}
		// 사망 모션
		float rotation_angle = (5 * (scene.time_stamp - spiderDeathStart) % 360) * TO_RADIAN;
		mat4 rottt = translate(mat4(1.0f), vec3(0.0f, 0.0f, 10.0f));
		rottt = rotate(rottt, rotation_angle, vec3(0.0f, 1.0f, 0.0f));

		vec3 tmppos = vec3(SpiderRoute[s_cidx - 1].rt.x, SpiderRoute[s_cidx - 1].rt.y, 0.0f);
		mat4 SpiderMM = translate(mat4(1.0f), tmppos);
		scene.DynamicModelMatrixes[1] = SpiderMM * RTM * rottt;
		

		// 사망 모션 끝 - init후 재시작. in ben
		if ((rotation_angle >= 180.0f*TO_RADIAN)) {
			flag.Spider_done = true;
			s_cidx = 0;
			spiderDeathStart = -1;
		}
		return;
	}
	
	Route& route = SpiderRoute[s_cidx];

	

	if (!s_turndone) {
		switch (route.dir) {
		case UP:
			angleRad = 2 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		case DOWN:

			break;
		case LEFT:
			angleRad = -1 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		case RIGHT:
			angleRad = 1 * 90.0f * TO_RADIAN;
			RTM = rotate(RTM, angleRad, vec3(0.0f, 0.0f, 1.0f));
			break;
		}
	}

	vec2 curr_pos = vec2(SpiderMM[3][0], SpiderMM[3][1]);
	//printf("%f %f\n", curr_pos.x, curr_pos.y);
	vec2 target_pos = vec2(route.rt.x, route.rt.y);
	vec2 difference = route.rt - curr_pos;
	float distance = length(curr_pos - target_pos);
	float step = 25.0f * DT;
	switch (route.dir) {
	case UP:
		if (difference.y <= 0) { // 보정
			SpiderMM = translate(SpiderMM, vec3(difference.x, difference.y, 0.0f));
			s_cidx++;
			s_turndone = false;
		}
		else {
			SpiderMM = translate(SpiderMM, vec3(0.0f, step, 0.0f));
		}
		break;
	case DOWN:
		if (difference.y >= 0) { // 보정
			SpiderMM = translate(SpiderMM, vec3(difference.x, difference.y, 0.0f));
			s_cidx++;
			s_turndone = false;
		}
		else {
			SpiderMM = translate(SpiderMM, vec3(0.0f, -step, 0.0f));
		}
		break;
	case LEFT:
		if (difference.x >= 0) { // 보정
			SpiderMM = translate(SpiderMM, vec3(difference.x, difference.y, 0.0f));
			s_cidx++;
			s_turndone = false;
		}
		else {
			SpiderMM = translate(SpiderMM, vec3(-step, 0.0f, 0.0f));
		}
		break;
	case RIGHT:
		if (difference.x <= 0) { // 보정
			SpiderMM = translate(SpiderMM, vec3(difference.x, difference.y, 0.0f));
			s_cidx++;
			s_turndone = false;
		}
		else {
			SpiderMM = translate(SpiderMM, vec3(step, 0.0f, 0.0f));
		}
		break;
	}
	scene.DynamicModelMatrixes[1] = SpiderMM * RTM;
}


std::random_device rd;
std::mt19937 gen(rd());

int Randint(){
	static std::uniform_int_distribution<> distInt(1, 4);
	return distInt(gen);
}


void init_BenRoute() {
	flag.Ben_done = flag.Spider_done = flag.Spider_start = flag.Ben_Die = false;
	FullRoute.clear();
	SpiderRoute.clear();

	FullRoute.insert(FullRoute.end(), R1.begin(), R1.end());
	BenMM = translate(mat4(1.0f), vec3(20.0f, 37.0f, 0.0f));

	SpiderRoute.insert(SpiderRoute.end(), SR1.begin(), SR1.end());
	SpiderMM = translate(mat4(1.0f), vec3(45.0f, 135.0f, 0.0f));

	switch (Randint()) {
	case 1:
		FullRoute.insert(FullRoute.end(), R2.begin(), R2.end());
		FullRoute.insert(FullRoute.end(), R3.begin(), R3.end());
		FullRoute.insert(FullRoute.end(), R4.begin(), R4.end());

		SpiderRoute.insert(SpiderRoute.end(), R2.begin(), R2.end());
		SpiderRoute.insert(SpiderRoute.end(), R3.begin(), R3.end());
		SpiderRoute.insert(SpiderRoute.end(), SR4.begin(), SR4.end());
		break;
	case 2:
		FullRoute.insert(FullRoute.end(), R2.begin(), R2.end());
		FullRoute.insert(FullRoute.end(), R3.begin(), R3.end());
		FullRoute.insert(FullRoute.end(), R4_1.begin(), R4_1.end());

		SpiderRoute.insert(SpiderRoute.end(), R2.begin(), R2.end());
		SpiderRoute.insert(SpiderRoute.end(), R3.begin(), R3.end());
		SpiderRoute.insert(SpiderRoute.end(), SR4_1.begin(), SR4_1.end());

		break;
	case 3:
		FullRoute.insert(FullRoute.end(), R2.begin(), R2.end());
		FullRoute.insert(FullRoute.end(), R3_1.begin(), R3_1.end());

		SpiderRoute.insert(SpiderRoute.end(), R2.begin(), R2.end());
		SpiderRoute.insert(SpiderRoute.end(), SR3_1.begin(), SR3_1.end());

		break;
	case 4:
		FullRoute.insert(FullRoute.end(), R2_1.begin(), R2_1.end());
		SpiderRoute.insert(SpiderRoute.end(), SR2_1.begin(), SR2_1.end());
		break;
	}
	b_size = FullRoute.size();
	s_size = SpiderRoute.size();
}




std::vector <Axis_Object> Axis_list;




void init_camera_axis() {
	scene.AxisMatrix = scale(mat4(1.0f), vec3(1.0f) * WC_AXIS_LENGTH);
	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().camera_id == CAMERA_MAIN || camera->get().camera_id == CAMERA_CC_0
			|| camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2
			|| camera->get().camera_id == CAMERA_DYNAMIC) {
			vec3 u = camera->get().cam_view.uaxis; 
			vec3 n = camera->get().cam_view.naxis;
			vec3 v = camera->get().cam_view.vaxis;
			vec3 pos = camera->get().cam_view.pos;

			mat4 R = mat4(1.0f);
			R[0] = vec4(u, 0.0f);
			R[1] = vec4(v, 0.0f);
			R[2] = vec4(n, 0.0f);
			R[3] = vec4(pos, 1.0f);
			R = scale(R, vec3(1.0f, 1.0f, 1.0f) * WC_AXIS_LENGTH);
			scene.CCAxisModelMatrixes.push_back(R);
		}
	}
	scene.CC_axis_object.resize(scene.CCAxisModelMatrixes.size());
	for (int i = 0; i < scene.CCAxisModelMatrixes.size(); i++) {
		scene.CC_axis_object[i].define_axis();
	}
}

void update_main_cam_axis(Camera &camera) {
	mat4 R = inverse(camera.ViewMatrix);
	vec3 pos = vec3(R[3][0], R[3][1], R[3][2]);
	camera.cam_view.pos = pos;
	camera.cam_view.uaxis = vec3(R[0]);
	camera.cam_view.vaxis = vec3(R[1]);
	camera.cam_view.naxis = vec3(R[2]);
	// 1 is EC light.
	// requset 4.
	lightList[1].position[0] = pos.x;
	lightList[1].position[1] = pos.y;
	lightList[1].position[2] = pos.z;
	vec3 forward = -normalize(glm::vec3(R[2]));
	lightList[1].spot_direction[0] = forward.x;
	lightList[1].spot_direction[1] = forward.y;
	lightList[1].spot_direction[2] = forward.z;
	R = scale(R, vec3(1.0f, 1.0f, 1.0f) * WC_AXIS_LENGTH);
	scene.CCAxisModelMatrixes[0] = R;
}

void update_dynamic_cam_axis(Camera& camera) {
	mat4 R = inverse(camera.ViewMatrix);
	R = scale(R, vec3(1.0f, 1.0f, 1.0f) * WC_AXIS_LENGTH);
	scene.CCAxisModelMatrixes[4] = R;
}



void display(void) {
	// To get shift key input
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().flag_valid == false) continue;
		//clear main camera
		if (camera-> get().camera_id == CAMERA_MAIN) { 
			
			if (flag.Camera_clear) {
				camera->get().ViewMatrix = lookAt(vec3(-600.0f, -600.0f, 400.0f), vec3(125.0f, 80.0f, 25.0f), vec3(0.0f, 0.0f, 1.0f));
				flag.Camera_clear = false;
			}
		}
		// Main cam Move
		if (!flag.Camera_Dynamic && camera->get().camera_id == CAMERA_MAIN && camera->get().flag_move) {
			
			vec3 front = -normalize(camera->get().cam_view.naxis);
			vec3 side = normalize(camera->get().cam_view.uaxis);
			vec3 top = normalize(camera->get().cam_view.vaxis);
			if (flag.Camera_rotate) {
				switch (flag.Camera_xyz) {
				case 1:
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, camera->get().cam_view.pos);
					camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, flag.rotate_toggle * 100 * DT * TO_RADIAN, front);
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, -camera->get().cam_view.pos);
					break;
				case 2:
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, camera->get().cam_view.pos);
					camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, flag.rotate_toggle * 100 * DT * TO_RADIAN, side);
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, -camera->get().cam_view.pos);
					break;
				case 3:
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, camera->get().cam_view.pos);
					camera->get().ViewMatrix = rotate(camera->get().ViewMatrix, flag.rotate_toggle * 100 * DT * TO_RADIAN, top);
					camera->get().ViewMatrix = translate(camera->get().ViewMatrix, -camera->get().cam_view.pos);
					break;
				default:
					break;
				}
				update_main_cam_axis(*camera);
			}
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
					case 4: //' '
						movement = top * 100.0f * DT;
						break;
					case 5: // z
						movement = -top * 100.0f * DT;
						break;
					}
					mat4 newmat = translate(mat4(1.0f), -movement);
					camera->get().ViewMatrix = camera->get().ViewMatrix * newmat;
					update_main_cam_axis(*camera);
				}
			}
		}
		
		if (flag.Camera_Dynamic && camera->get().camera_id == CAMERA_DYNAMIC) {
			mat4 oldVM = camera->get().ViewMatrix;
			mat4 R = rotate(mat4(1.0f), mouse.angle.y, vec3(1.0f, 0.0f, 0.0f));
			R = rotate(R, -mouse.angle.x, vec3(0.0f, 1.0f, 0.0f));
			R = transpose(R);
			R = R * oldVM;
			
			camera->get().ViewMatrix = R;
			
			//update.
			vec3 new_uaxis = vec3(R[0][0], R[1][0], R[2][0]);
			vec3 new_naxis = vec3(R[0][2], R[1][2], R[2][2]);
			vec3 new_vaxis = vec3(R[0][1], R[1][1], R[2][1]);
			camera->get().cam_view.naxis = new_naxis;
			camera->get().cam_view.uaxis = new_uaxis;
			camera->get().cam_view.vaxis = new_vaxis;
			
			vec3 front = normalize(-new_naxis);
			front.z = 0.0f;
			vec3 side = normalize(new_uaxis);
			side.z = 0.0f;
			vec3 updown = normalize(new_vaxis);
			updown.x = updown.y = 0.0f;
			update_dynamic_cam_axis(*camera);
			
		}

		glViewport(camera->get().view_port.x, camera->get().view_port.y,
			camera->get().view_port.w, camera->get().view_port.h);
		scene.ViewMatrix = camera->get().ViewMatrix;

		scene.ProjectionMatrix = camera->get().ProjectionMatrix;
		int s = FullRoute.size();
		//int ss = SpiderRoute.size();
		
		updateBenRoute();
		updateSpiderRoute();
		updateMMLight();
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
	case '0':
		scene.shader_kind = SHADER_SIMPLE;
	case '1':
		scene.shader_kind = SHADER_GOURAUD;
		for (int i = 0; i < lightList.size(); i++) {
			lightList[i].light_on = 0;
		}
		lightList[3].light_on = 1;
		break;
	case '2':
		scene.shader_kind = SHADER_PHONG;
		for (int i = 0; i < lightList.size(); i++) {
			lightList[i].light_on = 0;
		}
		lightList[3].light_on = 1;
		break;
	case '3':
		lightList[0].light_on = 1 - lightList[0].light_on;
		lightList[3].light_on = 0;
		break;
	case '4':
		lightList[1].light_on = 1 - lightList[1].light_on;
		lightList[3].light_on = 0;
		break;
	case '5':
		lightList[2].light_on = 1 - lightList[2].light_on;
		lightList[3].light_on = 0;
		break;
	/*case '6':
		scene.shader_kind = SHADER_PHONG_TEXTURE;
		break;*/


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
	case '[':
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
			fprintf(stdout, "Rotate Axis : X\n");
			flag.Camera_xyz = 1;
			break;
		}
		glutPostRedisplay();
		break;
	case 'y':
		// Change vm to y axis

		if (flag.Camera_rotate) {
			fprintf(stdout, "Rotate Axis : Y\n");
			flag.Camera_xyz = 2;
			break;
		}

		glutPostRedisplay();
		break;
	case 'z':
		// Rotate 우선
		if (flag.Camera_rotate) {
			fprintf(stdout, "Rotate Axis : Z\n");
			flag.Camera_xyz = 3;
			break;
		}
		if (!flag.Camera_Dynamic) {
			flag.camera_wasd[5] = true;
			break;
		}
		

		
		glutPostRedisplay();
		break;
	case 'p':
		flag.Camera_xyz = CAMERA_MAIN;
		flag.Camera_clear = true;
		flag.Camera_Ortho = false;
		for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
			if (camera->get().camera_id == CAMERA_MAIN || camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
				camera->get().flag_valid = true;
			}
			else {
				camera->get().flag_valid = false; // invalidate other cameras
			}
		}
		printf("^^^ Camera changed to main view & clear.\n");

		glutPostRedisplay();
		break;
	case ' ':
		if (!flag.Camera_Dynamic) {
			flag.camera_wasd[4] = true;
			break;
		}

		glutPostRedisplay();
		break;
	case 'r':
		if (flag.Camera_rotate) {
			fprintf(stdout, "Now Rotataion Disable.\n");
			flag.Camera_rotate = false;
		}
		else {
			fprintf(stdout, "Now Rotataion Available.\n");
			flag.Camera_rotate = true;
			flag.Camera_xyz = 0;
			flag.rotate_toggle = 0;
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
		// Dynamic cam mode 
		if (!flag.Camera_Dynamic) {
			fprintf(stdout, "Change Cam to Dynamic CCTV.\n");
			for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
				if (camera->get().camera_id == CAMERA_MAIN || camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
					camera->get().flag_valid = false;
				}
				if (camera->get().camera_id == CAMERA_ORTHO_X || camera->get().camera_id == CAMERA_ORTHO_Y || camera->get().camera_id == CAMERA_ORTHO_Z) {
					camera->get().flag_valid = true;
				}
				else if (camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
					camera->get().flag_valid = false;
				}

				else if (camera->get().camera_id == CAMERA_DYNAMIC) {
					camera->get().flag_valid = true; // invalidate other cameras
				}
				flag.Camera_Dynamic = true;
				flag.Camera_Ortho = true;
				//flag.Camera_Ortho = true;
			}
		}
		else {
			fprintf(stdout, "Change Cam to Main Cam & Move mode Available.\n");
			for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
				if (camera->get().camera_id == CAMERA_MAIN || camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
					camera->get().flag_valid = true;
				}
				if (camera->get().camera_id == CAMERA_ORTHO_X || camera->get().camera_id == CAMERA_ORTHO_Y || camera->get().camera_id == CAMERA_ORTHO_Z) {
					camera->get().flag_valid = false;
				}
				else if (camera->get().camera_id == CAMERA_CC_0 || camera->get().camera_id == CAMERA_CC_1 || camera->get().camera_id == CAMERA_CC_2) {
					camera->get().flag_valid = true;
				}
				else if (camera->get().camera_id == CAMERA_DYNAMIC){
					camera->get().flag_valid = false; // invalidate other cameras
				}
			}
			flag.Camera_Dynamic = false;
			flag.Camera_Ortho = false;
		}
		
		break;
	case ']':
		// Show orthogonal Cam on CCTV window.
		fprintf(stdout, "Show orthogonal XYZ Cam.\n");
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
			fprintf(stdout, "Show CCTVs.\n");
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
	case ';':
		if (!scene.axistoggle) {
			fprintf(stdout, "Show XYZ Axis.\n");
			scene.axistoggle = true;
		}
		else {
			fprintf(stdout, "Show UVN Axis.\n");
			scene.axistoggle = false;
		}
		break;
	/*case 'N': case 'n':
		fuckcat = true;
		break;
	case 'L': case 'l':
		fuckcat = false;
		break;*/


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
		if (!flag.Camera_Dynamic )
			flag.camera_wasd[4] = false;
		break;
	case 'z':
		if (!flag.Camera_Dynamic) {
			flag.camera_wasd[5] = false;
		}
		break;
	case 'x':
		break;
	case 'y':
		break;
	}
}

void mouseWheel(int wheel, int direction, int x, int y) {
	float zoomf = 1.0f;
	if (direction < 0) {
		// 휠을 위로 굴렸을 때 (줌 인)
		zoomf = 1.1f;
	}
	else {
		// 휠을 아래로 굴렸을 때 (줌 아웃)
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
	if (flag.Camera_Dynamic) {
		if (flag.First) {
			centerMouse();
			mouse.angle = vec2(-90.0f, 0.0f);
			mouse.lastPoint = vec2(0.0f,0.0f);
			flag.First = false;
			return;
		}
		vec2 offset = mouse.point - mouse.lastPoint;
		mouse.lastPoint = vec2(0.0f,0.0f);
		offset *= 0.001f;
		//printf("%f %f\n", mouse.lastPoint.x, mouse.lastPoint.y);
		mouse.angle = offset;
		//printf("%f %f\n", mouse.angle.x, mouse.angle.y);
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
			if(flag.Camera_rotate)
				flag.rotate_toggle = -1;
		}
		if (state == GLUT_UP) {
			mouse.leftPressed = false;
			flag.rotate_toggle = 0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse.rightPressed = true;
			if (flag.Camera_rotate) {
				flag.rotate_toggle = 1;
			}
		}
		if (state == GLUT_UP) {
			mouse.rightPressed = false;
			flag.rotate_toggle = 0;
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
	fprintf(stdout, "    This program was coded by 20211607 HanYuSeung\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = " I HATE SPIDER ";
	char messages[N_MESSAGE_LINES][256] = { "KEY USE: \nW/A/S/D/Space/Z : Move Main Cam \nZoom in/out : Wheel \nRotation Mode: R\n    Choose Rotate Axis : X/Y/Z\n    Rotate By Chosen Axis : Mouse left/right click\nDynamic Cam : M\nClear to Main cam: p\nOrthoProjection: 1\nChange XYZ - UNV : L\n"  };

	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	init_camera_axis();
	Init_route();
	glutMainLoop();
}
