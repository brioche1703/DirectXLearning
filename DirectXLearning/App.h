#pragma once

#include "Window.h"
#include "Timer.h"
#include "BindableCommon.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"

#include <set>

class App {
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();
	void ShowModelWindow();

private:
	ImguiManager imguiManager;
	Camera cam;
	Window wnd;
	Timer timer;

	PointLight light;
	float speed_factor = 1.0f;

	Model nano{ wnd.Gfx(), "src\\models\\nanosuit.obj" };

	struct {
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;
};
