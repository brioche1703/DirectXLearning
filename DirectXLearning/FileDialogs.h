#pragma once

#include <Windows.h>
#include <string>
#include <filesystem>

class FileDialogs {
public:
	static std::filesystem::path OpenFile(HWND hWnd) {
		OPENFILENAME ofn = { 0 };
		TCHAR szFile[260] = { 0 };
		TCHAR szFileTitle[260] = { 0 };
		// Initialize remaining fields of OPENFILENAME structure
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrInitialDir = "src\\models";
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Obj files (*.obj)|*.obj|All files (*.*)|*.*";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = szFileTitle;
		ofn.nMaxFileTitle = sizeof(szFileTitle);
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == TRUE) {
			return std::filesystem::path(ofn.lpstrFile);
		}

		return "null";
	}

};
