#include "IncludeWin.h"
#include "App.h"

#include <fstream>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try {
		return App{ lpCmdLine }.Go();
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