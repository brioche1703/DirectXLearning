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
	App(const std::string& commandLine = "");
	~App();

	int Go();

private:
	void DoFrame();
	void ShowImguiDemoWindow();

private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imguiManager;
	Camera cam;
	Window wnd;
	Timer timer;

	PointLight light;
	float speed_factor = 1.0f;

	Model goblin{ wnd.Gfx(), "src\\models\\gobber\\GoblinX.obj" , 6.0f };
	Model nano{ wnd.Gfx(), "src\\models\\nano_textured\\nanosuit.obj", 2.0f };
	TestPlane tp{ wnd.Gfx(), 6.0 };
	Model wall{ wnd.Gfx(), "src\\models\\brick_wall\\brick_wall.obj", 2.0f };
	Model sponza{ wnd.Gfx(), "src\\models\\Sponza\\sponza.obj", 1.0f / 20.0f };
};
