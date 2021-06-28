#include "FileDialogs.h"
#include "SceneHierarchyPanel.h"

#include "Scene.h"
#include "Window.h"

#include <cstdio>

extern Ecs::Coordinator gCoordinator;

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<SceneSystem> sceneSystem)
	:
	sceneSystem(sceneSystem) {
}

void SceneHierarchyPanel::SpawnPanel(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg) noexcept {
	ImGui::Begin("Scene Hierarchy");

	Ecs::Entity entityToDelete = {};
	for (auto entity : sceneSystem->entities) {
		auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);

		ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Leaf;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, e->GetName().c_str());
		if (ImGui::IsItemClicked(0)) {
			selectedEntity = entity;
		}

		if (auto model = std::dynamic_pointer_cast<Model>(e)) {
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete")) {
					entityToDelete = entity;
				}
				ImGui::EndPopup();
			}
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

	if (entityToDelete) {
		gCoordinator.DestroyEntity(entityToDelete);
		if (selectedEntity == entityToDelete) {
			selectedEntity = -1;
		}
	}

	if (ImGui::BeginPopupContextWindow(0, 1, false)) {
		if (ImGui::MenuItem("Add Model")) {
			std::filesystem::path path = FileDialogs::OpenFile(wnd->GetWindowH());
			Scene::AddModel(path.filename().string(), gfx, rg, path.string());
		}
		if (ImGui::MenuItem("Add Camera")) {
			Scene::AddCamera(std::make_shared<Camera>(gfx, "New Camera"), rg);
		}
		ImGui::EndPopup();
	}

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		selectedEntity = -1;
	}

	ImGui::End();
}
