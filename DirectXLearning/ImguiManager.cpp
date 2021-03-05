#include "ImguiManager.h"

#include "external/imgui/imgui.h"

ImguiManager::ImguiManager() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager() {
	ImGui::DestroyContext();
}
