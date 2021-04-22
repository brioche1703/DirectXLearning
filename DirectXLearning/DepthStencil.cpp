#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"
#include "Graphics.h"
#include "RenderTarget.h"

namespace wrl = Microsoft::WRL;

namespace Bind {
	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput) {
		INFOMAN(gfx);

		// Create depth stencil texture 
		wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&descDepth,
			nullptr,
			&pDepthStencil));

		// Create depth stencil target view 
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
			pDepthStencil.Get(),
			nullptr,
			&pDepthStencilView));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx) noexcept {
		GetContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noexcept {
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* rt) noexcept {
		rt->BindAsBuffer(gfx, this);
	}

	void DepthStencil::Clear(Graphics& gfx) noexcept {
		GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot)
		:
		ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot)
	{}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot)
		:
		DepthStencil(gfx, width, height, true)
	{
		INFOMAN(gfx);

		wrl::ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pRes.Get(),
			&srvDesc,
			&pShaderResourceView));

	}

	void ShaderInputDepthStencil::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pShaderResourceView.GetAddressOf());
	}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		:
		OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		:
		DepthStencil(gfx, width, height, false)
	{}

	void OutputOnlyDepthStencil::Bind(Graphics& gfx) noexcept {
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}
