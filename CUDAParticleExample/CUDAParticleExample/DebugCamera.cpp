#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <minmax.h>
#include <iostream>
#include "GameTime.h"
#include "DebugCamera.h"

bool keyDown(int key) {
	return (GetKeyState(key) & 0x80) != 0;
}

void DebugCamera::updateView() {
	updateFromInput();
	transform = getRotationMatrix()*Matrix4::translate(-pos);
}

void DebugCamera::updateFromInput() {
	float time = GameTime::getFrameTime();
	if (keyDown(VK_LEFT))
		azimuth += az_speed*time;
	if (keyDown(VK_RIGHT))
		azimuth += -az_speed*time;
	if (keyDown(VK_DOWN))
		altitude = max(-0.5f*(float)M_PI, altitude - al_speed*time);
	if (keyDown(VK_UP))
		altitude = min(0.5f*(float)M_PI, altitude + al_speed*time);

	Matrix4 rot = getRotationMatrix().transpose();
	
	if (keyDown('W'))
		pos = pos + rot*Vector3(0, 0, -mv_speed*time);
	if (keyDown('S'))
		pos = pos - rot*Vector3(0, 0, -mv_speed*time);
	
	if (keyDown('A'))
		pos = pos - rot*Vector3(mv_speed*time, 0, 0);
	if (keyDown('D'))
		pos = pos + rot*Vector3(mv_speed*time, 0, 0);
	
	if (keyDown('Q'))
		pos = pos + rot*Vector3(0, mv_speed*time, 0);
	if (keyDown('E'))
		pos = pos - rot*Vector3(0, mv_speed*time, 0);

	if (keyDown(VK_ADD)) {
		if (!plusdown) {
			plusdown = true;
			mv_speed *= 1.3f;
		}
	}
	else plusdown = false;
	if (keyDown(VK_SUBTRACT)) {
		if (!minusdown) {
			minusdown = true;
			mv_speed /= 1.3f;
		}
	}
	else minusdown = false;
}

Matrix4 DebugCamera::getRotationMatrix() {
	return Matrix4::rotateAboutXAxis(-altitude)*Matrix4::rotateAboutYAxis(-azimuth);
}