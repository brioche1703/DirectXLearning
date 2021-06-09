#pragma once

#include <string>
#include <Windows.h>

class FileDialogs {
public:
	static std::string OpenFile(HWND hWnd) {
		OPENFILENAME ofn = { 0 };
		TCHAR szFile[260] = { 0 };
		// Initialize remaining fields of OPENFILENAME structure
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrInitialDir = "src\\models";
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Obj files (*.obj)|*.obj|All files (*.*)|*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}

		return "null";
	}

private:

};
