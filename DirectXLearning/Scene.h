#pragma once

#include "Model.h"
#include "Camera.h"
#include "ECS.h"
#include "TestModelProbe.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "SceneHierarchyPanel.h"

#include "external/imgui/imgui.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

#define MAX_NUM_LIGHTS 10

class Graphics;
class SceneSystem;
class LightSystem;
class ModelSystem;
class Window;

namespace Rgph {
	class RenderGraph;
}

class Scene {
public:
	Scene();

	void Submit(size_t channel);

	static void AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale = 1.0f) noexcept;
	static void AddCamera(std::shared_ptr<Camera> pCam, Rgph::RenderGraph& rg) noexcept;
	static void AddPointLight(std::shared_ptr<PointLight> pLight, Rgph::RenderGraph& rg) noexcept;

	CameraContainer& GetCameraContrainer() noexcept;
	std::shared_ptr<Model> GetModel(std::string name) noexcept;
	std::shared_ptr<PointLight> GetLight(std::string name) noexcept;
	void BindLights(Graphics& gfx) const noexcept;

	void SpawnProbeWindow(std::string name) noexcept;
	void SpawnHierarchyPanel(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg) noexcept;

	void ScalingTest(float dt);

private:
	std::string name;
	std::shared_ptr<SceneSystem> sceneSystem;
	std::shared_ptr<LightSystem> lightSystem;
	std::shared_ptr<ModelSystem> modelSystem;
	SceneHierarchyPanel sceneHierarchyPanel;

	static CameraContainer cameraContainer;
};
