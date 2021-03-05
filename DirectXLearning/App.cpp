#include "App.h"
#include "Pyramid.h"
#include "Melon.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "GDIPlusManager.h"
#include "Cylinder.h"
#include "ModelTest.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <sstream>
#include <iomanip>

GDIPlusManager gfipm;

App::App()
	:
	wnd(1280, 1024, "DirectX Learning"),
	light(wnd.Gfx())
{
	class Factory {
	public:
		Factory(Graphics& gfx) : gfx(gfx) {}

		std::unique_ptr<Drawable> operator()() {
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					tdist);
			case 1:
				return std::make_unique<SkinnedBox>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist);
			case 2:
				const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
				return std::make_unique<Box>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					bdist,
					mat);
			case 3:
				return std::make_unique<Cylinder>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					bdist,
					tdist);
			case 4:
				const DirectX::XMFLOAT3 matModel = { cdist(rng), cdist(rng), cdist(rng) };
				return std::make_unique<ModelTest>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					matModel,
					1.0f);
			default:
				assert(false && "Bad drawable type in factory!");
				break;
			}
		}

	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 4, 4 };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> tdist{ 3, 30 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(drawableCount);
	std::generate_n(std::back_inserter(drawables), drawableCount, f);

	for (auto& pd : drawables) {
		if (auto pb = dynamic_cast<ModelTest*>(pd.get())) {
			boxes.push_back(pb);
		}
	}

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
	for (auto& b : drawables)
	{
		b->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0 : dt);
		b->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	SpawnSimulationWindow();
	SpawnBoxWindowManager();
	SpawnBoxWindows();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept {
	if (ImGui::Begin("Simulation Speed")) {
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 14.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
}

void App::SpawnBoxWindowManager() noexcept {
	if (ImGui::Begin("Boxes")) {
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str())) {
			for (int i = 0; i < boxes.size(); i++) {
				const bool selected = *comboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected)) {
					comboBoxIndex = i;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept {
	for (auto i = boxControlIds.begin(); i != boxControlIds.end();) {
		if (!boxes[*i]->SpawnControlWindow(*i, wnd.Gfx())) {
			i = boxControlIds.erase(i);
		}
		else {
			i++;
		}
	}
}
