#include "Scene.h"

#include "Model.h"
#include "TestModelProbe.h"
#include "ModelProbe.h"
#include "Utils.h"
#include "Camera.h"

extern Ecs::Coordinator gCoordinator;

Scene::Scene() {
	gCoordinator.RegisterComponent<std::shared_ptr<Model>>();
	gCoordinator.RegisterComponent<std::shared_ptr<MP>>();

	sceneSystem = gCoordinator.RegisterSystem<SceneSystem>();
	{
		Ecs::Signature signature;
		signature.set(gCoordinator.GetComponentType<std::shared_ptr<Model>>());
		signature.set(gCoordinator.GetComponentType<std::shared_ptr<MP>>());
		gCoordinator.SetSystemSignature<SceneSystem>(signature);
	}
	sceneSystem->Init();
}

void Scene::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& entity : sceneSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		model->LinkTechniques(rg);
	}
}

void Scene::Submit(size_t channel) noxnd {
	for (auto& entity : sceneSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		model->Submit(channel);
	}
}

void Scene::AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale) noexcept {
	auto title = RemoveFileExtension(name);
	auto m = std::make_shared<Model>(gfx, title, path, scale);
	auto mp = std::make_shared<MP>(title);
	m->LinkTechniques(rg);

	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, m);
	gCoordinator.AddComponent(entity, mp);
}

std::shared_ptr<Model> Scene::GetModel(std::string name) noexcept {
	for (auto& entity : sceneSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		if (model->GetName() == name) {
			return model;
		}
	}
}

void Scene::SpawnProbeWindow(std::string name) noexcept {
	for (auto& entity : sceneSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		if (model->GetName() == name) {
			auto& mp = gCoordinator.GetComponent<std::shared_ptr<MP>>(entity);
			mp->SpawnWindow(*model);
		}
	}
}

void Scene::SpawnAllProbeWindow() noexcept {
	for (auto& entity : sceneSystem->entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		auto& mp = gCoordinator.GetComponent<std::shared_ptr<MP>>(entity);
		mp->SpawnWindow(*model);
	}
}

void Scene::ScalingTest(float dt) {
	sceneSystem->Update(dt);
}
