#pragma once

#include "Graphics.h"
#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : GraphicsResource {
public:
	RenderTarget(Graphics& gfx, UINT width, UINT height);
	void BindAsTexture(Graphics& gfx, UINT slot) const noexcept;
	void BindAsTarget(Graphics& gfx) const noexcept;
	void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept;
	void Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept;
	void Clear(Graphics& gfx) const noexcept;


private:
	UINT width; 
	UINT height;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
};
