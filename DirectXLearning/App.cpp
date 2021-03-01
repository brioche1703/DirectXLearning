#include "App.h"
#include "Pyramid.h"
#include "Melon.h"
#include "MathsUtils.h"
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "GDIPlusManager.h"

#include <sstream>
#include <iomanip>

GDIPlusManager gfipm;

App::App()
	:
	wnd(1280, 1024, "DirectX Learning")
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
					rdist);
			case 1:
				return std::make_unique<SkinnedBox>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist);
			case 2:
				return std::make_unique<Melon>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					longdist,
					latdist);
			case 3:
				return std::make_unique<Sheet>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist);
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
		std::uniform_int_distribution<int> typedist{ 0, 3 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(drawableCount);
	std::generate_n(std::back_inserter(drawables), drawableCount, f);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{}

int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}
		DoFrame();
		if (wnd.kbd.KeyIsPressed(VK_ESCAPE)) {
			return 0;
		}
	}
}

void App::DoFrame() {
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : drawables)
	{
		b->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0 : dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}