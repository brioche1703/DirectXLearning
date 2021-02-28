#pragma once

#include "Window.h"
#include "Timer.h"
#include "BindableBase.h"

class App {
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();

private:
	Window wnd;
	Timer timer;

	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t drawableCount = 180;
};
