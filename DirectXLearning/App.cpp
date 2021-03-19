#include "App.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "VertexBuffer.h"
#include "NormalMapTwerker.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include <sstream>
#include <iomanip>
#include <shellapi.h>

GDIPlusManager gfipm;

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "DirectX Learning"),
	light(wnd.Gfx())
{
	if (this->commandLine != "") {
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
		if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--ntwerk-rotx180") {
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			NormalMapTwerker::RotateXAxis180(
				std::string(pathInWide.begin(), pathInWide.end()),
				std::string(pathOutWide.begin(), pathOutWide.end())
			);
			throw std::runtime_error("Normal map processed successfully.");
		}
	}

	wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	tp.SetPos({ 12.0f,0.0f,0.0f });
	goblin.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
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

	goblin.Draw(wnd.Gfx());
	nano.Draw(wnd.Gfx());
	wall.Draw(wnd.Gfx());
	tp.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	while (const auto e = wnd.kbd.ReadKey()) {
		if (!e->IsPress()) {
			continue;
		}
		switch (e->GetCode()) {
		case 'Q':
			if (wnd.CursorEnabled()) {
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else {
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = !showDemoWindow;
			break;
		}
	}

	if (!wnd.CursorEnabled()) {
		if (wnd.kbd.KeyIsPressed('W')) {
			cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S')) {
			cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('A')) {
			cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('D')) {
			cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R')) {
			cam.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F')) {
			cam.Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			cam.Rotate(delta->x, delta->y);
		}
	}

	// IMGUI
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	goblin.ShowWindow(wnd.Gfx(), "Goblin");
	nano.ShowWindow(wnd.Gfx(), "Nanosuit");
	wall.ShowWindow(wnd.Gfx(), "Wall");
	tp.SpawnControlWindow(wnd.Gfx());

	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
