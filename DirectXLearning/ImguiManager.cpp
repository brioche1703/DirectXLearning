#include "ImguiManager.h"

#include "FileDialogs.h"
#include "Window.h"
#include "Scene.h"
#include "Graphics.h"
#include "Model.h"

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

void ImguiManager::ShowMainMenuBar(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg) {
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl+N")) {
			}
			if (ImGui::MenuItem("Open...", "Ctrl+O")) {
				std::filesystem::path path = FileDialogs::OpenFile(wnd->GetWindowH());
				Scene::AddModel(path.filename().string(), gfx, rg, path.string());
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void ImguiManager::Toggle() noexcept {
	enabled = !enabled;
}

bool ImguiManager::IsEnabled() const noexcept {
	return enabled;
}
