#pragma once

#include "Graphics.h"
#include "Bindable.h"

class PixelShader : public Bindable {
public:
	PixelShader(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};
