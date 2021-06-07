#include "ImguiManager.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_win32.h"
#include "external/imgui/imgui_impl_dx11.h"

ImguiManager::ImguiManager() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

ImguiManager::~ImguiManager() {
	ImGui::DestroyContext();
}

void ImguiManager::NewFrame() {
	if (enabled) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	}
}

void ImguiManager::EndFrame() {
	if (enabled) {
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ImguiManager::ShowDemoWindow() {
	ImGui::ShowDemoWindow();
}

void ImguiManager::Toggle() noexcept {
	enabled = !enabled;
}

bool ImguiManager::IsEnabled() const noexcept {
	return enabled;
}
