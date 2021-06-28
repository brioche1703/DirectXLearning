#include "Scene.h"

#include "Model.h"
#include "TestModelProbe.h"
#include "ModelProbe.h"
#include "Utils.h"
#include "Camera.h"
#include "SceneEntity.h"
#include "LightSystem.h"
#include "ModelSystem.h"
#include "CameraSystem.h"

extern Ecs::Coordinator gCoordinator;

Scene::Scene() {
	// Scene system
	{
		gCoordinator.RegisterComponent<std::shared_ptr<SceneEntity>>();

		sceneSystem = gCoordinator.RegisterSystem<SceneSystem>();
		{
			Ecs::Signature signature;
			signature.set(gCoordinator.GetComponentType<std::shared_ptr<SceneEntity>>());
			gCoordinator.SetSystemSignature<SceneSystem>(signature);
		}

		sceneHierarchyPanel = SceneHierarchyPanel(sceneSystem);
	}
	// Light system
	{
		gCoordinator.RegisterComponent<std::shared_ptr<PointLight>>();

		lightSystem = gCoordinator.RegisterSystem<LightSystem>();
		{
			Ecs::Signature signature;
			signature.set(gCoordinator.GetComponentType<std::shared_ptr<PointLight>>());
			gCoordinator.SetSystemSignature<LightSystem>(signature);
		}
	}
	// Model system
	{
		gCoordinator.RegisterComponent<std::shared_ptr<Model>>();

		modelSystem = gCoordinator.RegisterSystem<ModelSystem>();
		{
			Ecs::Signature signature;
			signature.set(gCoordinator.GetComponentType<std::shared_ptr<Model>>());
			gCoordinator.SetSystemSignature<ModelSystem>(signature);
		}
	}
}

void Scene::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& entity : lightSystem->entities) {
		auto& e = gCoordinator.GetComponent<std::shared_ptr<PointLight>>(entity);
		e->LinkTechniques(rg);
	}
}

void Scene::Submit(size_t channel) noxnd {
	for (auto& entity : sceneSystem->entities) {
		auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
		e->Submit(channel);
	}
}

void Scene::AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale) noexcept {
	auto title = RemoveFileExtension(name);
	auto model = std::make_shared<Model>(gfx, title, path, scale);
	model->LinkTechniques(rg);

	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, std::shared_ptr<SceneEntity>(model));
	gCoordinator.AddComponent(entity, std::shared_ptr<Model>(model));
}

void Scene::AddCamera(std::shared_ptr<Camera> pCam, Rgph::RenderGraph& rg) noexcept {
	auto entity = gCoordinator.CreateEntity();
	pCam->LinkTechniques(rg);
	gCoordinator.AddComponent(entity, std::shared_ptr<SceneEntity>(pCam));
	cameraContainer.AddCamera(pCam);
}

void Scene::AddPointLight(std::shared_ptr<PointLight> pLight) noexcept {
	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, std::shared_ptr<SceneEntity>(pLight));
	gCoordinator.AddComponent(entity, pLight);
	cameraContainer.AddCamera(pLight->ShareCamera());
}

CameraContainer& Scene::GetCameraContrainer() noexcept {
	return cameraContainer;
}

std::shared_ptr<Model> Scene::GetModel(std::string name) noexcept {
	for (auto& entity : modelSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		if (model->GetName() == name) {
			return model;
		}
	}
}

std::shared_ptr<PointLight> Scene::GetLight(std::string name) noexcept {
	for (auto& entity : lightSystem->entities) {
		auto& light = gCoordinator.GetComponent<std::shared_ptr<PointLight>>(entity);
		if (light->GetName() == name) {
			return light;
		}
	}
}

void Scene::SpawnProbeWindow(std::string name) noexcept {
	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
			if (e->GetName() == name) {
				e->OnImguiRender(true);
			}
		}
	}
}

void Scene::SpawnHierarchyPanel(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg) noexcept {
	sceneHierarchyPanel.SpawnPanel(wnd, gfx, rg);
}

void Scene::ScalingTest(float dt) {
	sceneSystem->Update(dt);
}

CameraContainer Scene::cameraContainer;
