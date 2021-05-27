#pragma once

#include "Bindable.h"
#include "DepthStencil.h"

#include <memory>
#include <vector>

namespace Bind {
	class OutputOnlyDepthStencil;
	class OutputOnlyRenderTarget;

	class CubeTexture : public Bindable {
	public:
		CubeTexture(Graphics& gfx, const std::string& path, UINT slot = 0);
		void Bind(Graphics& gfx) noxnd override;

	private:
		unsigned int slot;

	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};

	class CubeTargetTexture : public Bindable {
	public:
		CubeTargetTexture(Graphics& gfx, UINT width, UINT height, UINT slot = 0, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);
		void Bind(Graphics& gfx) noxnd override;
		std::shared_ptr<OutputOnlyRenderTarget> GetRenderTarget(size_t index) const;

	private:
		unsigned int slot;

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyRenderTarget>> renderTargets;
	};

	class DepthCubeTexture : public Bindable {
	public:
		DepthCubeTexture(Graphics& gfx, UINT size, UINT slot = 0);
		void Bind(Graphics& gfx) noxnd override;
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer(size_t index) const;

	private:
		unsigned int slot;

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>> depthBuffers;
	};

	class DepthCubeSingleTexture : public Bindable {
	public:
		DepthCubeSingleTexture(Graphics& gfx, UINT size, UINT slot = 0);
		void Bind(Graphics& gfx) noxnd override;
		std::shared_ptr<ShaderInputDepthStencil> GetDepthBuffer() const;

	protected:
		std::shared_ptr<ShaderInputDepthStencil> depthBuffer;
	};
}
