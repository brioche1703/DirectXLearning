#pragma once

#include "IncludeWin.h"
#include <d3d11.h>

class Graphics {
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept {
		const float color[] = { red, green, blue };
		pContext->ClearRenderTargetView(pTarget, color);
	}

private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView *pTarget = nullptr;
};
