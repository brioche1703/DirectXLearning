#pragma once

#include "ECS.h"
#include "SceneSystem.h"

#include "external/imgui/imgui.h"

#include <memory>

class Window;

extern Ecs::Coordinator gCoordinator;

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(std::shared_ptr<SceneSystem> sceneSystem);

	void SpawnPanel(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg) noexcept;

private:
	std::shared_ptr<SceneSystem> sceneSystem;
	Ecs::Entity selectedEntity = {};
};

