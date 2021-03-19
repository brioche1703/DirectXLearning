#pragma once

#include "Window.h"
#include "Timer.h"
#include "BindableCommon.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include "TestPlane.h"

#include <set>

class App {
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();
	void ShowImguiDemoWindow();

private:
	bool showDemoWindow = false;
	ImguiManager imguiManager;
	Camera cam;
	Window wnd;
	Timer timer;

	PointLight light;
	float speed_factor = 1.0f;

	Model goblin{ wnd.Gfx(), "src\\models\\gobber\\GoblinX.obj" , 6.0f };
	Model nano{ wnd.Gfx(), "src\\models\\nano_textured\\nanosuit.obj", 6.0f };
	TestPlane tp{ wnd.Gfx(), 6.0 };
	Model wall{ wnd.Gfx(), "src\\models\\brick_wall\\brick_wall.obj", 2.0f };
};
