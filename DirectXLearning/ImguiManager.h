#pragma once

class Window;
class Graphics;

namespace Rgph {
	class RenderGraph;
}

class ImguiManager
{
public:
	ImguiManager();
	~ImguiManager();

	void NewFrame();
	void EndFrame();
	void ShowDemoWindow();
	void ShowMainMenuBar(Window* wnd, Graphics& gfx, Rgph::RenderGraph& rg);

	void Toggle() noexcept;
	bool IsEnabled() const noexcept;

private:
	bool enabled = true;
};
