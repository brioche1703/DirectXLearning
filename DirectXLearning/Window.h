#pragma once

#include "IncludeWin.h"
#include "DirectXException.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <optional>

class Window {
public:
	class Exception : public DirectXException {
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		virtual const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		
	private:
		HRESULT hr;
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
	static std::optional<int> ProcessMessages();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
};

// Error exception macro helper
#define DXWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define DXWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())
