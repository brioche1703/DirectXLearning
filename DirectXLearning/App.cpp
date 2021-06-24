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
	wnd(1280, 720, "DirectX Learning"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), { 10.0f, 18.0f, 0.0f }) {
	cameras.AddCamera(std::make_shared<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, PI / 2.0f));
	cameras.AddCamera(std::make_shared<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f, 28.8f, -6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	cameras.AddCamera(light.ShareCamera());

	scene.AddCamera(std::make_shared<Camera>(wnd.Gfx(), "C", dx::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, 0.0f, PI / 2.0f));

	TestDynamicMeshLoading(wnd.Gfx());

	scene.AddModel("goblin", wnd.Gfx(), rg, "src\\models\\gobber\\GoblinX.obj", 4.0f);
	scene.AddModel("nano", wnd.Gfx(), rg, "src\\models\\nano_textured\\nanosuit.obj", 1.5f);
	scene.AddModel("sponza", wnd.Gfx(), rg, "src\\models\\Sponza\\sponza.obj", 1.0f / 20.0f);
	
	scene.GetModel("nano")->SetRootTransform(
		dx::XMMatrixRotationY(PI / 2.0f) *
		dx::XMMatrixTranslation(27.0f, -0.56f, 1.7f)
	);
	scene.GetModel("goblin")->SetRootTransform(
		dx::XMMatrixRotationY(-PI / 2.0f) *
		dx::XMMatrixTranslation(-8.0f, 10.0f, 0.0f)
	);

	light.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());
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

	imguiManager.NewFrame();
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(Chan::main);
	cameras.Submit(Chan::main);
	scene.Submit(Chan::main);
	scene.Submit(Chan::shadow);

	rg.Execute(wnd.Gfx());

	if (savingDepth) {
		rg.DumpShadowMap(wnd.Gfx(), "shadowSingle.png");
		savingDepth = false;
	}

	//scene.ScalingTest(dt);
	//cameras.TravelingTest(dt);

	// IMGUI
	if (imguiManager.IsEnabled()) {
		scene.SpawnAllProbeWindow();
		cameras.SpawnWindow(wnd.Gfx());
		light.SpawnControlWindow("Light 1");
		rg.RenderWindows(wnd.Gfx());
	}

	imguiManager.ShowMainMenuBar(&wnd, wnd.Gfx(), rg);
	imguiManager.EndFrame();
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
			cameras->Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.kbd.KeyIsPressed('S')) {
			cameras->Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.kbd.KeyIsPressed('A')) {
			cameras->Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('D')) {
			cameras->Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R')) {
			cameras->Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F')) {
			cameras->Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			cameras->Rotate((float)delta->x, (float)delta->y);
		}
	}

}
