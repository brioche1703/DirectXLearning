#include "App.h"
#include "Utils.h"
#include "Testing.h"
#include "ModelProbe.h"
#include "TestModelProbe.h"
#include "MathsUtils.h"
#include "Camera.h"
#include "Channels.h"

namespace dx = DirectX;

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "DirectX Learning"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx(), { 10.0f, 18.0f, 0.0f })
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f, 28.8f, -6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	cameras.AddCamera(light.ShareCamera());

	TestDynamicMeshLoading(wnd.Gfx());

	nano.SetRootTransform(
		dx::XMMatrixRotationY(PI / 2.0f) *
		dx::XMMatrixTranslation(27.0f, -0.56f, 1.7f)
	);
	goblin.SetRootTransform(
		dx::XMMatrixRotationY(-PI / 2.0f) *
		dx::XMMatrixTranslation(-8.0f, 10.0f, 0.0f)
	);

	models.push_back(&nano);
	models.push_back(&goblin);
	models.push_back(&sponza);

	light.LinkTechniques(rg);
	cameras.LinkTechniques(rg);
	for(auto m : models) {
		m->LinkTechniques(rg);
	}

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

void App::DoFrame(float dt) {

	imguiManager.NewFrame();
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(Chan::main);
	cameras.Submit(Chan::main);
	for (auto m : models) {
		m->Submit(Chan::main);
		m->Submit(Chan::shadow);
	}

	rg.Execute(wnd.Gfx());

	if (savingDepth) {
		rg.DumpShadowMap(wnd.Gfx(), "shadowSingle.png");
		savingDepth = false;
	}

	static MP sponzaProbe{ "Sponza" };
	static MP goblinProbe{ "Goblin" };
	static MP nanoProbe{"Nano"};

	// IMGUI
	if (imguiManager.IsEnabled()) {
		sponzaProbe.SpawnWindow(sponza);
		goblinProbe.SpawnWindow(goblin);
		nanoProbe.SpawnWindow(nano);

		cameras.SpawnWindow(wnd.Gfx());
		light.SpawnControlWindow("Light 1");
		rg.RenderWindows(wnd.Gfx());
	}

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
