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

class Graphics;
class SceneSystem;
class LightSystem;
class ModelSystem;

namespace Rgph {
	class RenderGraph;
}

class Scene {
public:
	Scene();

	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel);

	static void AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale = 1.0f) noexcept;
	void AddCamera(std::shared_ptr<Camera> pCam) noexcept;
	void AddPointLight(std::shared_ptr<PointLight> pLight) noexcept;

	CameraContainer& GetCameraContrainer() noexcept;
	std::shared_ptr<Model> GetModel(std::string name) noexcept;
	std::shared_ptr<PointLight> GetLight(std::string name) noexcept;

	void SpawnProbeWindow(std::string name) noexcept;
	void SpawnHierarchyPanel() noexcept;

	void ScalingTest(float dt);

private:
	std::string name;
	std::shared_ptr<SceneSystem> sceneSystem;
	std::shared_ptr<LightSystem> lightSystem;
	std::shared_ptr<ModelSystem> modelSystem;
	SceneHierarchyPanel sceneHierarchyPanel;
	CameraContainer cameraContainer;
};
