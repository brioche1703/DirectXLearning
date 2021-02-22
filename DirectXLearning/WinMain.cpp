#include "Window.h"
#include "DirectXException.h"

int CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow) 
{
	try {
		Window wnd(800, 300, "DirextX Learning");

		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (wnd.kbd.KeyIsPressed(VK_SPACE)) {
				MessageBox(nullptr, "Space pressed!", "Space pressed", MB_OK | MB_ICONEXCLAMATION);
			}
		}
;
		if (gResult == -1) {
			return -1;
		}

		return msg.wParam;
	}
	catch (const DirectXException& e) {
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, "No Details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}