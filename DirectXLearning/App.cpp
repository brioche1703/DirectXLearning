#include "App.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "VertexBuffer.h"
#include "NormalMapTwerker.h"
#include "Utils.h"
#include "DynamicConstant.h"
#include "Testing.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <shellapi.h>
#include <dxtex/DirectXTex.h>
#include <memory>

namespace dx = DirectX;

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1920, 1080, "DirectX Learning"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx())
{
	//TestDynamicConstant();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());
	//goblin.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1080.0f / 1920.0f, 0.5f, 400.0f));
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

	//goblin.Draw(wnd.Gfx());
	//nano.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	//sponza.Draw(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());
	tc1.Draw(wnd.Gfx());
	tc1.DrawOutline(wnd.Gfx());
	//tc1.DrawOutline(wnd.Gfx());

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
	tc1.SpawnControlWindow(wnd.Gfx(), "Cube");
	ShowImguiDemoWindow();

	//goblin.ShowWindow(wnd.Gfx(), "Goblin");
	//nano.ShowWindow(wnd.Gfx(), "Nanosuit");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

	//sponza.ShowWindow(wnd.Gfx(), "Sponza");

	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
