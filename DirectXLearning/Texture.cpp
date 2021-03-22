#include "Texture.h"
#include "Surface.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind {
	namespace wrl = Microsoft::WRL;

	Texture::Texture(Graphics& gfx, const std::string& path, UINT slot) 
		:
		path(path),
		slot(slot) {
		INFOMAN(gfx);

		const auto s = Surface::FromFile(path);
		hasAlpha = s.AlphaLoaded();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		wrl::ComPtr<ID3D11Texture2D> pTexture;

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&textureDesc, nullptr, &pTexture));

		GetContext(gfx)->UpdateSubresource(
			pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0u);

		D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = {};
		textureViewDesc.Format = textureDesc.Format;
		textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		textureViewDesc.Texture2D.MostDetailedMip = 0;
		textureViewDesc.Texture2D.MipLevels = -1;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &textureViewDesc, &pTextureView));

		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}

	void Texture::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot) {
		return Codex::Resolve<Texture>(gfx, path, slot);
	}

	std::string Texture::GenerateUID(const std::string& path, UINT slot) {
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}

	std::string Texture::GetUID() const noexcept {
		return GenerateUID(path, slot);
	}

	bool Texture::HasAlpha() const noexcept {
		return hasAlpha;
	}
}
