#pragma once

#include "Model.h"
#include "Camera.h"
#include "ECS.h"
#include "TestModelProbe.h"

#include "external/imgui/imgui.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

class Graphics;

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

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;

	SceneHierarchyPanel(std::shared_ptr<SceneSystem> sceneSystem) 
		:
		sceneSystem(sceneSystem)
	{}

	void SpawnPanel() {
		ImGui::Begin("Scene Hierarchy");

		for (auto& entity : sceneSystem->entities) {
			if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
				auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);

				ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
				flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
				flags |= ImGuiTreeNodeFlags_Leaf;
				bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, e->GetName().c_str());
				if (ImGui::IsItemClicked()) {
					selectedEntity = entity;
				}

				ImGui::Begin("Properties");
				if (selectedEntity == entity) {
					e->OnImguiRender(true);
				}
				else {
					e->OnImguiRender(false);
				}

				ImGui::End();

				if (opened) {
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}

private:
	std::shared_ptr<SceneSystem> sceneSystem;
	Ecs::Entity selectedEntity = {};
};

class Scene {
public:
	Scene();

	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel);

	static void AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale = 1.0f) noexcept;
	void AddCamera(std::shared_ptr<Camera> pCam) noexcept;
	std::shared_ptr<Model> GetModel(std::string name) noexcept;
	void SpawnProbeWindow(std::string name) noexcept;
	void SpawnAllProbeWindow() noexcept;

	void ScalingTest(float dt);

private:
	std::string name;
	std::shared_ptr<SceneSystem> sceneSystem;
	SceneHierarchyPanel sceneHierarchyPanel;
};
