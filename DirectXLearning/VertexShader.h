#pragma once

#include "Bindable.h"
#include "Graphics.h"

class VertexShader : public Bindable {
public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
};
