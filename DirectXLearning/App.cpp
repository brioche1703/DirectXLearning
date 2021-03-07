#include "App.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "GDIPlusManager.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include <sstream>
#include <iomanip>

GDIPlusManager gfipm;

App::App()
	:
	wnd(1280, 1024, "DirectX Learning"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{}

int App::Go() {
	while (true) {
		if (wnd.kbd.KeyIsPressed(VK_ESCAPE)) {
			return 0;
		}
		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame() {
	auto dt = timer.Mark() * speed_factor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());

	light.Bind(wnd.Gfx(), cam.GetMatrix());

	const auto transform =
		DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw)
		* DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	nano.Draw(wnd.Gfx(), transform);

	light.Draw(wnd.Gfx());

	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow() {
	if (ImGui::Begin("Model")) {
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderAngle("X", &pos.x, -120.0f, 120.0f);
		ImGui::SliderAngle("Y", &pos.y, -120.0f, 120.0f);
		ImGui::SliderAngle("Z", &pos.z, -120.0f, 120.0f);
	}
	ImGui::End();
}
