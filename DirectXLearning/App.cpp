#include "App.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "VertexBuffer.h"
#include "NormalMapTwerker.h"
#include "Utils.h"
#include "DynamicConstant.h"
#include "Testing.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "Node.h"
#include "XMUtils.h"
#include "TechniqueProbe.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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
	wnd(1280, 720, "DirectX Learning"),
	scriptCommander(TokenizeQuoted(commandLine)),
	light(wnd.Gfx())
{
	TestDynamicMeshLoading(wnd.Gfx());

	tc1.SetPos({ 4.0f, 0.0f, 0.0f });
	tc2.SetPos({ 0.0f, 4.0f, 0.0f });

	{
		{
			auto bcp = std::make_unique<BufferClearPass>("clear");
			bcp->SetInputResource("renderTarget", "$.backbuffer");
			bcp->SetInputResource("depthStencil", "$.masterDepth");
			rg.AppendPass(std::move(bcp));
		}
		{
			auto lp = std::make_unique<LambertianPass>("lambertian");
			lp->SetInputResource("renderTarget", "clear.renderTarget");
			lp->SetInputResource("depthStencil", "clear.depthStencil");
			rg.AppendPass(std::move(lp));
		}
		rg.SetSinkTarget("backbuffer", "lambertian.renderTarget");
		rg.Finalize();

		tc1.LinkTechniques(rg);
		tc2.LinkTechniques(rg);
		light.LinkTechniques(rg);
	}

	//TestDynamicConstant();

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());
	//goblin.SetRootTransform(DirectX::XMMatrixRotationY(-146.0f) * DirectX::XMMatrixTranslation(-10.0f, 8.4f, -4.0f));
	//nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
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

	//goblin.Submit(fc);
	//nano.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	light.Submit();
	tc1.Submit();
	//sponza.Submit(fc);
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());
	tc2.Submit();

	rg.Execute(wnd.Gfx());

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
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	class TP : public TechniqueProbe {
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
			if (auto v = buf["offset"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Offset"), &v, 0.0f, 1.0f, "%.3f", 2.5f));
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
			if (auto v = buf["useSpecularMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
			}
			if (auto v = buf["useNormalMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	};

	class MP : ModelProbe {
	public:
		void SpawnWindow(Model& model, const char* title) {
			ImGui::Begin(title);
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr) {
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
				if (dirty)
				{
					pSelectedNode->SetAppliedTransform(
						dx::XMMatrixRotationX(tf.xRot) *
						dx::XMMatrixRotationY(tf.yRot) *
						dx::XMMatrixRotationZ(tf.zRot) *
						dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
					);
				}

			}
			if (pSelectedNode != nullptr) {
				TP probe;
				pSelectedNode->Accept(probe);
			}
			ImGui::End();
		}

	protected:
		bool PushNode(Node& node) override {
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			const auto node_flags = 
				ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);

			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags, node.GetName().c_str()
			);

			if (ImGui::IsItemClicked()) {
				struct Probe : public TechniqueProbe {
					virtual void OnSetTechnique() {
						if (pTech->GetName() == "Outline") {
							pTech->SetActiveState(highlighted);
						}
					}
					bool highlighted = false;
				} probe;

				if (pSelectedNode != nullptr) {
					pSelectedNode->Accept(probe);
				}
				probe.highlighted = true;
				node.Accept(probe);
				pSelectedNode = &node;
			}

			return expanded;
		}

		void PopNode(Node& node) override {
			ImGui::TreePop();
		}

	private:
		Node* pSelectedNode = nullptr;

		struct TransformParameters {
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};

		std::unordered_map<int, TransformParameters> transformParams;

	private:
		TransformParameters& ResolveTransform() noexcept {
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if (i == transformParams.end()) {
				return LoadTransform(id);
			}
			return i->second;
		}

		TransformParameters& LoadTransform( int id ) noexcept {
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles( applied );
			const auto translation = ExtractTranslation( applied );
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert( { id,{ tp } } ).first->second;
		}


	};
	static MP modelProbe;

	// IMGUI
	//modelProbe.SpawnWindow(sponza, "Sponza");
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	tc1.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	tc2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	ShowImguiDemoWindow();

	//modelProbe.SpawnWindow(goblin, "Goblin");
	//nano.ShowWindow(wnd.Gfx(), "Nanosuit");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

	wnd.Gfx().EndFrame();
	rg.Reset();
}

void App::ShowImguiDemoWindow() {
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
