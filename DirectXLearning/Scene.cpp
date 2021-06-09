#include "Scene.h"
#include "Model.h"
#include "TestModelProbe.h"
#include "ModelProbe.h"

Scene::Scene() {}

void Scene::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& m : models) {
		m.second.get()->LinkTechniques(rg);
	}
}

void Scene::Submit(size_t channel) noxnd {
	for (auto& m : models) {
		m.second.get()->Submit(channel);
	}
}

void Scene::AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale) noexcept {
	auto m = std::make_shared<Model>(gfx, name, path, scale);
	auto mp = std::make_shared<MP>(name);
	m->LinkTechniques(rg);
	models.emplace(name, m);
	modelsProbes.emplace(name, mp);
}

std::shared_ptr<Model> Scene::GetModel(std::string name) noexcept {
	return models.at(name);
}

void Scene::SpawnProbeWindow(std::string name) noexcept {
	modelsProbes.at(name)->SpawnWindow(*models.at(name));
}

void Scene::SpawnAllProbeWindow() noexcept {
	for (auto& p : modelsProbes) {
		p.second->SpawnWindow(*models.at(p.first));
	}
}

std::map<std::string, std::shared_ptr<Model>> Scene::models;
std::map<std::string, std::shared_ptr<MP>> Scene::modelsProbes;
