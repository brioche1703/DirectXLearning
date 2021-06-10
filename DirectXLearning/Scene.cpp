#include "Scene.h"

#include "Model.h"
#include "TestModelProbe.h"
#include "ModelProbe.h"
#include "Utils.h"

Scene::Scene() {}

void Scene::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& m : models) {
		m.second.first.get()->LinkTechniques(rg);
	}
}

void Scene::Submit(size_t channel) noxnd {
	for (auto& m : models) {
		m.second.first.get()->Submit(channel);
	}
}

void Scene::AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale) noexcept {
	auto title = RemoveFileExtension(name);
	title = GenerateUID(title, models);
	auto m = std::make_shared<Model>(gfx, title, path, scale);
	auto mp = std::make_shared<MP>(title);
	m->LinkTechniques(rg);
	models.emplace(title, std::make_pair(m, mp));
}

std::shared_ptr<Model> Scene::GetModel(std::string name) noexcept {
	return models.at(name).first;
}

void Scene::SpawnProbeWindow(std::string name) noexcept {
	models.at(name).second->SpawnWindow(*models.at(name).first);
}

void Scene::SpawnAllProbeWindow() noexcept {
	for (auto& p : models) {
		p.second.second->SpawnWindow(*models.at(p.first).first);
	}
}

std::map<std::string, std::pair<std::shared_ptr<Model>, std::shared_ptr<MP>>> Scene::models;
