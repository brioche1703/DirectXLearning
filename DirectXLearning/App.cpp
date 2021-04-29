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
	light(wnd.Gfx(), { 10.0f, 5.0f, 0.0f })
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ -13.5f, 6.0f, 3.5f }, 0.0f, PI / 2.0f));
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f, 28.8f, -6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	cameras.AddCamera(light.ShareCamera());

	TestDynamicMeshLoading(wnd.Gfx());

	tc1.SetPos({ 10.0f, 5.0f, 6.0f });
	tc2.SetPos({ 10.0f, 5.0f, 14.0f });
	nano.SetRootTransform(
		dx::XMMatrixRotationY(PI / 2.0f) *
		dx::XMMatrixTranslation(27.0f, 0.56f, 1.7f)
	);
	goblin.SetRootTransform(
		dx::XMMatrixRotationY(-PI / 2.0f) *
		dx::XMMatrixTranslation(-8.0f, 10.0f, 0.0f)
	);

	tc1.LinkTechniques(rg);
	tc2.LinkTechniques(rg);
	light.LinkTechniques(rg);
	sponza.LinkTechniques(rg);
	nano.LinkTechniques(rg);
	goblin.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());
	//TestDynamicConstant();
	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());
	//goblin.SetRootTransform(DirectX::XMMatrixRotationY(-146.0f) * DirectX::XMMatrixTranslation(-10.0f, 8.4f, -4.0f));
	//nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));
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

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(Chan::main);
	tc1.Submit(Chan::main);
	tc2.Submit(Chan::main);
	sponza.Submit(Chan::main);
	goblin.Submit(Chan::main);
	nano.Submit(Chan::main);
	cameras.Submit(Chan::main);

	tc1.Submit(Chan::shadow);
	tc2.Submit(Chan::shadow);
	sponza.Submit(Chan::shadow);
	goblin.Submit(Chan::shadow);
	nano.Submit(Chan::shadow);

	rg.Execute(wnd.Gfx());

	if (savingDepth) {
		rg.DumpShadowMap(wnd.Gfx(), "shadow.png");
		savingDepth = false;
	}

	//wall.Draw(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());

	static MP sponzaProbe{ "Sponza" };
	static MP goblinProbe{ "Goblin" };
	static MP nanoProbe{"Nano"};

	// IMGUI
	sponzaProbe.SpawnWindow(sponza);
	goblinProbe.SpawnWindow(goblin);
	nanoProbe.SpawnWindow(nano);

	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();
	tc1.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	tc2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	rg.RenderWidgets(wnd.Gfx());
	ShowImguiDemoWindow();

	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

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
			showDemoWindow = !showDemoWindow;
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

void App::ShowImguiDemoWindow() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
