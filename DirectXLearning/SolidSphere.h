#pragma once
#include "Drawable.h"


class SolidSphere : public Drawable {
public:
	SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color = { 0.5f, 0.5f, 0.5f });
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 color = { 0.5f, 0.5f, 0.5f };
};
