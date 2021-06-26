#include "SceneHierarchyPanel.h"

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<SceneSystem> sceneSystem)
	:
	sceneSystem(sceneSystem) {
}

void SceneHierarchyPanel::SpawnPanel() {
	ImGui::Begin("Scene Hierarchy");

	for (auto& entity : sceneSystem->entities) {
		if (gCoordinator.HasComponent<std::shared_ptr<SceneEntity>>()) {
			auto& e = gCoordinator.GetComponent<std::shared_ptr<SceneEntity>>(entity);

			ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			flags |= ImGuiTreeNodeFlags_Leaf;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, e->GetName().c_str());
			if (ImGui::IsItemClicked(0)) {
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
