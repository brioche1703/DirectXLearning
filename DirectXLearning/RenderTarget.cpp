#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"

#include <array>

namespace wrl = Microsoft::WRL;

RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height) {
		INFOMAN(gfx);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		wrl::ComPtr<ID3D11Texture2D> pTexture;

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&textureDesc, nullptr, &pTexture));


		// View
		D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
		textureViewDesc.Format = textureDesc.Format;
		textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		textureViewDesc.Texture2D.MostDetailedMip = 0;
		textureViewDesc.Texture2D.MipLevels = 1;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &textureViewDesc, &pTextureView));

		// Target view
		D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
		targetViewDesc.Format = textureDesc.Format;
		targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		targetViewDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(
			pTexture.Get(), &targetViewDesc, &pTargetView));
}

void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot) const noexcept {
	GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(Graphics& gfx) const noexcept {
	GetContext(gfx)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept {
	GetContext(gfx)->OMSetRenderTargets(1, pTargetView.GetAddressOf(), depthStencil.pDepthStencilView.Get());
}

void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept {
	GetContext(gfx)->ClearRenderTargetView(pTargetView.Get(), color.data());
}

void RenderTarget::Clear(Graphics& gfx) const noexcept {
	Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f });
}
