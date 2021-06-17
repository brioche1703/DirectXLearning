#pragma once

#include "Model.h"
#include "ECS.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

class Graphics;
class MP;

namespace Rgph {
	class RenderGraph;
}

extern Ecs::Coordinator gCoordinator;

class SceneSystem : public Ecs::System {
public:
	void Init() {

	}

	void Update() {
		static float scale = 0.1f;
		for (auto const& entity : entities) {
			auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
			//model->SetRootTransform(DirectX::XMMatrixScaling(scale, scale, scale));
		}
		scale += 0.1f;
	}
};

class Scene {
public:
	Scene();

	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel);

	static void AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale = 1.0f) noexcept;
	std::shared_ptr<Model> GetModel(std::string name) noexcept;
	void SpawnProbeWindow(std::string name) noexcept;
	void SpawnAllProbeWindow() noexcept;

private:

	std::shared_ptr<SceneSystem> sceneSystem;
};
