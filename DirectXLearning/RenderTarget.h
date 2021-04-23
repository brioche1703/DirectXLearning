#pragma once

#include "Bindable.h"
#include "BufferResource.h"

class Graphics;

namespace Bind {
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource {
	public:
		void BindAsBuffer(Graphics& gfx) noxnd override;
		void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd override;
		void BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noxnd;
		void Clear(Graphics& gfx) noxnd override;
		void Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd;
		UINT GetWidth() const noxnd;
		UINT GetHeight() const noxnd;

	private:
		void BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd;

	protected:
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
		RenderTarget(Graphics& gfx, UINT width, UINT height);

		UINT width;
		UINT height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	};

	class ShaderInputRenderTarget : public RenderTarget {
	public:
		ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot);
		void Bind(Graphics& gfx) noxnd override;

	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	class OutputOnlyRenderTarget : public RenderTarget {
		friend Graphics;
	public:
		void Bind(Graphics& gfx) noxnd override;

	private:
		OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
	};
}
