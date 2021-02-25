#include "App.h"

#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "DirectX Learning") {}

int App::Go() {
	while (!quit) {
		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}
		DoFrame();
		if (wnd.kbd.KeyIsPressed(VK_ESCAPE)) {
			quit = true;
		}
	}
}

void App::DoFrame() {
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);

	wnd.Gfx().DrawTriangle(
		-timer.Peek(),
		0.0f,
		0.0f);

	wnd.Gfx().DrawTriangle(
		timer.Peek(), 
		wnd.mouse.GetPosX() / 400.0f - 1.0f, 
		-wnd.mouse.GetPosY() / 300.0f + 1.0f);

	wnd.Gfx().EndFrame();
}