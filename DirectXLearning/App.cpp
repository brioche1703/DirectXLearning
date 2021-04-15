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
	TestDynamicMeshLoading(wnd.Gfx());

	tc1.SetPos({ 4.0f, 0.0f, 0.0f });
	tc1.SetPos({ 0.0f, 4.0f, 0.0f });

	{
		std::string path = "src\\models\\brick_wall\\brick_wall.obj";
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);

		Material mat{ wnd.Gfx(), *pScene->mMaterials[1], path };
		pLoaded = std::make_unique<Mesh>(wnd.Gfx(), mat, *pScene->mMeshes[0]);
	}

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
	light.Submit(fc);
	//sponza.Draw(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());
	//tc1.Submit(fc);
	//tc2.Submit(fc);
	pLoaded->Submit(fc, DirectX::XMMatrixIdentity());

	fc.Execute(wnd.Gfx());

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

	class Probe : public TechniqueProbe
	{
	public:
		void OnSetTechnique() override {
			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
			bool active = pTech->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
			pTech->SetActiveState(active);
		}

		bool OnVisitBuffer(Dcb::Buffer& buf) override {
			namespace dx = DirectX;
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
			(const char* label) mutable {
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto v = buf["scale"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f));
			}
			if (auto v = buf["materialColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
			}
			if (auto v = buf["specularWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useNormalMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	} probe;
	pLoaded->Accept(probe);

	// IMGUI
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	tc1.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	tc2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	ShowImguiDemoWindow();

	//goblin.ShowWindow(wnd.Gfx(), "Goblin");
	//nano.ShowWindow(wnd.Gfx(), "Nanosuit");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

	//sponza.ShowWindow(wnd.Gfx(), "Sponza");

	wnd.Gfx().EndFrame();
	fc.Reset();
}

void App::ShowImguiDemoWindow() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
