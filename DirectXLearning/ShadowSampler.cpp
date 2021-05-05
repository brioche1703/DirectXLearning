#include "ShadowSampler.h"

#include "GraphicsThrowMacros.h"

Bind::ShadowSampler::ShadowSampler(Graphics& gfx) {
	for (size_t i = 0; i < 4; i++) {
		curSampler = i;
		pSamplers[i] = MakeSampler(gfx, GetBilinear(), GetHardwarePcf());
	}
	SetBilinear(true);
	SetHardwarePcf(false);
	Bind(gfx);
	SetHardwarePcf(true);
	Bind(gfx);
}

void Bind::ShadowSampler::Bind(Graphics& gfx) noxnd {
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(GetCurrentSlot(), 1, pSamplers[curSampler].GetAddressOf()));
}

void Bind::ShadowSampler::SetBilinear(bool bilin) {
	curSampler = (curSampler & ~0b01) | (bilin ? 0b01 : 0);
}

void Bind::ShadowSampler::SetHardwarePcf(bool hwPcf) {
	curSampler = (curSampler & ~0b10) | (hwPcf ? 0b10 : 0);
}

bool Bind::ShadowSampler::GetBilinear() const {
	return curSampler & 0b01;
}

bool Bind::ShadowSampler::GetHardwarePcf() const {
	return curSampler & 0b10;
}

UINT Bind::ShadowSampler::GetCurrentSlot() const {
	return GetHardwarePcf() ? 1 : 2;
}

size_t Bind::ShadowSampler::ShadowSamplerIndex(bool bilin, bool hwPcf) {
	return (bilin ? 0b01 : 0) + (hwPcf ? 0b10 : 0);

}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Bind::ShadowSampler::MakeSampler(Graphics& gfx, bool bilin, bool hwPcf) {
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

	if (hwPcf) {
		samplerDesc.Filter = bilin ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	}
	else {
		samplerDesc.Filter = bilin ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	return std::move(pSampler);
}
