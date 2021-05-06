#include "CubeTexture.h"

#include "GraphicsThrowMacros.h"
#include "Surface.h"

namespace Bind {
	namespace wrl = Microsoft::WRL;

	Bind::CubeTexture::CubeTexture(Graphics& gfx, const std::string& path, UINT slot)
		:
		path(path),
		slot(slot)
	{
		INFOMAN(gfx);
	
		std::vector<Surface> surfaces;
		for (int i = 0; i < 6; i++) {
			surfaces.push_back(Surface::FromFile(path + "\\" + std::to_string(i) + ".png"));
		}
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = surfaces[0].GetWidth();
		texDesc.Height = surfaces[0].GetHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 6;
		texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA data[6];
		for (int i = 0; i < 6; i++) {
			data[i].pSysMem = surfaces[i].GetBufferPtrConst();
			data[i].SysMemPitch = surfaces[i].GetBytePitch();
			data[i].SysMemSlicePitch = 0;
		}

		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, data, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	}

	void Bind::CubeTexture::Bind(Graphics& gfx) noxnd {
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf()));
	}
}
