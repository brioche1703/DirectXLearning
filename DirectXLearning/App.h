#pragma once

#include "Window.h"
#include "Timer.h"
#include "BindableCommon.h"
#include "ImguiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "Model.h"
#include "TestPlane.h"
#include "TestCube.h"
#include "ScriptCommander.h"
#include "Material.h"
#include "ScaleOutlineRenderGraph.h"
#include "BlurOutlineRenderGraph.h"
#include "MathsUtils.h"
#include "CameraContainer.h"

#include <set>

class App {
public:
	App(const std::string& commandLine = "");
	~App();

	int Go();

private:
	void DoFrame(float dt);
	void HandleInput(float dt);
	void ShowImguiDemoWindow();

private:
	std::string commandLine;
	bool showDemoWindow = false;
	ImguiManager imguiManager;
	Window wnd;
	ScriptCommander scriptCommander;
	Timer timer;
	float speed_factor = 1.0f;
	CameraContainer cameras;
	Rgph::BlurOutlineRenderGraph rg{ wnd.Gfx() };

	PointLight light;
	Model goblin{ wnd.Gfx(), "src\\models\\gobber\\GoblinX.obj" , 4.0f };
	Model nano{ wnd.Gfx(), "src\\models\\nano_textured\\nanosuit.obj", 2.0f };
	Model sponza{ wnd.Gfx(), "src\\models\\Sponza\\sponza.obj", 1.0f / 20.0f };
	//Model wall{ wnd.Gfx(), "src\\models\\brick_wall\\brick_wall.obj", 2.0f };
	//TestPlane bluePlane{ wnd.Gfx(), 6.0 , {0.3f, 0.3f, 1.0f, 0.5f}};
	//TestPlane redPlane{ wnd.Gfx(), 6.0 , {1.0f, 0.3f, 0.3f, 0.5f}};
	TestCube tc1{ wnd.Gfx(), 4.0f };
	TestCube tc2{ wnd.Gfx(), 4.0f };
	TestPlane tp{ wnd.Gfx(), 50.0f, {0.8f, 0.8f, 0.8f, 1.0f}};

	bool savingDepth = true;
};
