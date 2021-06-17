#pragma once

#include "Model.h"
#include "Camera.h"
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
	void Init() {}

	void Update(float dt) {
		for (auto const& entity : entities) {
			auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
			DirectX::XMFLOAT4X4 f = model->GetRootTransform();
			DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&f);
			if (model->GetName() != "sponza") {
				model->SetRootTransform(
					DirectX::XMMatrixScaling(1.001f, 1.001f, 1.001f) *
					m
					);
			}
		}
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

	void ScalingTest(float dt);

private:

	std::shared_ptr<SceneSystem> sceneSystem;
};
