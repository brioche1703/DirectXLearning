#include "App.h"
#include "Utils.h"
#include "Testing.h"
#include "ModelProbe.h"
#include "TestModelProbe.h"
#include "MathsUtils.h"
#include "Camera.h"
#include "Channels.h"
#include "ECS.h"

Ecs::Coordinator gCoordinator;

namespace dx = DirectX;


App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1920, 1080, "DirectX Learning"),
	scriptCommander(TokenizeQuoted(commandLine)) 
{
	scene.AddCamera(std::make_shared<Camera>(wnd.Gfx(), "Camera A", dx::XMFLOAT3{ 0.0f, 8.0f, 0.0f }, 0.0f, PI / 2.0f), rg);
	scene.AddCamera(std::make_shared<Camera>(wnd.Gfx(), "Camera B", dx::XMFLOAT3{ -13.5f, 28.8f, -6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f), rg);

	scene.AddModel("Nano", wnd.Gfx(), rg, "src\\models\\nano_textured\\nanosuit.obj", 1.5f);
	scene.AddModel("Sponza", wnd.Gfx(), rg, "src\\models\\Sponza\\sponza.obj", 1.0f / 15.0f);

	scene.AddPointLight(std::make_shared<PointLight>(wnd.Gfx(), "Light 1", dx::XMFLOAT3{ 10.0f, 18.0f, 0.0f }), rg);
	scene.AddPointLight(std::make_shared<PointLight>(wnd.Gfx(), "Light 2", dx::XMFLOAT3{ -10.0f, 18.0f, 0.0f }), rg);
	scene.AddPointLight(std::make_shared<PointLight>(wnd.Gfx(), "Light 3", dx::XMFLOAT3{ 0.0f, 18.0f, 3.0f }), rg);

	rg.BindShadowCamera(*scene.GetLight("Light 1")->ShareCamera());
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
		auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		DoFrame(dt);
	}
}

HWND App::GetWindowHandler() const noexcept {
	return wnd.GetWindowH();
}

void App::DoFrame(float dt) {

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	scene.BindLights(wnd.Gfx(), scene.GetCameraContainer()->GetMatrix());
	rg.BindMainCamera(scene.GetCameraContainer().GetActiveCamera());
	scene.GetCameraContainer().TravelingTest(dt);

	scene.Submit(Chan::main);
	scene.Submit(Chan::shadow);

	rg.Execute(wnd.Gfx());

	if (savingDepth) {
		rg.DumpShadowMap(wnd.Gfx(), "shadowSingle.png");
		savingDepth = false;
	}

	// IMGUI
	if (imguiManager.IsEnabled()) {
		imguiManager.NewFrame();

		scene.SpawnHierarchyPanel(&wnd, wnd.Gfx(), rg);
		scene.GetCameraContainer().SpawnWindow(wnd.Gfx());
		rg.RenderWindows(wnd.Gfx());

		imguiManager.ShowMainMenuBar(&wnd, wnd.Gfx(), rg);
		//imguiManager.ShowDemoWindow();
		imguiManager.EndFrame();
	}

	wnd.Gfx().EndFrame();
	rg.Reset();
}

void App::HandleInput(float dt) {
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
			imguiManager.Toggle();
			break;
		case 'P':
			savingDepth = true;
			break;
		}
	}

	if (!wnd.CursorEnabled()) {
		if (wnd.kbd.KeyIsPressed('W')) {
			scene.GetCameraContainer()->Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S')) {
			scene.GetCameraContainer()->Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('A')) {
			scene.GetCameraContainer()->Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('D')) {
			scene.GetCameraContainer()->Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R')) {
			scene.GetCameraContainer()->Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F')) {
			scene.GetCameraContainer()->Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			scene.GetCameraContainer()->Rotate((float)delta->x, (float)delta->y);
		}
	}

}
