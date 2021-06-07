#pragma once

class ImguiManager
{
public:
	ImguiManager();
	~ImguiManager();

	void NewFrame();
	void EndFrame();
	void ShowDemoWindow();

	void Toggle() noexcept;
	bool IsEnabled() const noexcept;

private:
	bool enabled = true;
};
