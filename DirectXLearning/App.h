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
#include "Scene.h"

#include <set>

class App {
public:
	App(const std::string& commandLine = "");
	~App();

	int Go();

	HWND GetWindowHandler() const noexcept;

private:
	void DoFrame(float dt);
	void HandleInput(float dt);

private:
	std::string commandLine;
	ImguiManager imguiManager;
	Window wnd;
	ScriptCommander scriptCommander;
	Timer timer;
	float speed_factor = 1.0f;
	Rgph::BlurOutlineRenderGraph rg{ wnd.Gfx() };

	Scene scene;

	bool savingDepth = true;
};
