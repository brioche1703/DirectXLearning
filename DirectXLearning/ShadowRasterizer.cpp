#include "ShadowRasterizer.h"
#include "GraphicsThrowMacros.h"

Bind::ShadowRasterizer::ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp) {
	ChangeDepthBiasParameters(gfx, depthBias, slopeBias, clamp);
}

void Bind::ShadowRasterizer::ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp) {
	this->depthBias = depthBias;
	this->slopeBias = slopeBias;
	this->clamp = clamp;

	INFOMAN(gfx);

	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.DepthBias = depthBias;
	rasterDesc.SlopeScaledDepthBias = slopeBias;
	rasterDesc.DepthBiasClamp = clamp;

	GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
}

void Bind::ShadowRasterizer::Bind(Graphics& gfx) noxnd {
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(pRasterizer.Get()));
}

int Bind::ShadowRasterizer::GetDepthBias() const {
	return depthBias;
}

float Bind::ShadowRasterizer::GetSlopBias() const {
	return slopeBias;
}

float Bind::ShadowRasterizer::GetClamp() const {
	return clamp;
}
