#pragma once

#include "Window.h"
#include "Timer.h"
#include "BindableBase.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"

#include <set>

class App {
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();

	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManager() noexcept;
	void SpawnBoxWindows() noexcept;

private:
	ImguiManager imguiManager;
	Camera cam;
	Window wnd;
	Timer timer;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class ModelTest*> boxes;
	static constexpr size_t drawableCount = 180;

	PointLight light;
	float speed_factor = 1.0f;

	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};
