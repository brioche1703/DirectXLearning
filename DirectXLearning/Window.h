#pragma once

#include "IncludeWin.h"
#include "DirectXException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "WindowThrowMacros.h"

#include <optional>
#include <memory>

class Window {
public:
	class Exception : public DirectXException {
		using DirectXException::DirectXException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	class HrException : public Exception {
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
		
	private:
		HRESULT hr;
	};

	class NoGfxException : public Exception {
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};


private:
	// Singleton manages registration/cleanup of window class
	class WindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "DirectX Learning Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	HWND GetWindowH() const noexcept;

private:
	void FreeCursor() noexcept;
	void ConfineCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	BOOL WindowResize(HWND hWnd, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = false;
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
	std::string commandLine;
};
