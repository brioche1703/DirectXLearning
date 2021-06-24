#include "Scene.h"

#include "Model.h"
#include "TestModelProbe.h"
#include "ModelProbe.h"
#include "Utils.h"
#include "Camera.h"
#include "SceneEntity.h"

extern Ecs::Coordinator gCoordinator;

Scene::Scene() {
	gCoordinator.RegisterComponent<std::shared_ptr<SceneEntity>>();

	sceneSystem = gCoordinator.RegisterSystem<SceneSystem>();
	{
		Ecs::Signature signature;
		signature.set(gCoordinator.GetComponentType<std::shared_ptr<SceneEntity>>());
		gCoordinator.SetSystemSignature<SceneSystem>(signature);
	}
	sceneSystem->Init();

	sceneHierarchyPanel = SceneHierarchyPanel(sceneSystem);
}

void Scene::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
			e->LinkTechniques(rg);
		}
	}
}

void Scene::Submit(size_t channel) noxnd {
	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& model = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
			model->Submit(channel);
		}
	}
}

void Scene::AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale) noexcept {
	auto title = RemoveFileExtension(name);
	auto m = std::make_shared<Model>(gfx, title, path, scale);
	m->LinkTechniques(rg);

	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, std::shared_ptr<SceneEntity>(m));
}

void Scene::AddCamera(std::shared_ptr<Camera> pCam) noexcept {
	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, std::shared_ptr<SceneEntity>(pCam));
	gCoordinator.AddComponent(entity, pCam);
}

std::shared_ptr<Model> Scene::GetModel(std::string name) noexcept {
	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
			std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(e);
			if (model != nullptr) {
				if (model->GetName() == name) {
					return model;
				}
			}
		}
	}
}

void Scene::SpawnProbeWindow(std::string name) noexcept {
	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);
			std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(e);
		}
	}
}

void Scene::SpawnAllProbeWindow() noexcept {
	sceneHierarchyPanel.SpawnPanel();
}

void Scene::ScalingTest(float dt) {
	sceneSystem->Update(dt);
}
