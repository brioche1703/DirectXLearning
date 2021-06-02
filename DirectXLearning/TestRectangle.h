#pragma once

#include "Drawable.h"

class TestRectangle : public Drawable {
public:
	TestRectangle(Graphics& gfx, float size)
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx, const std::string& title) noexcept;

private:
	struct PSMaterialConstant {
		DirectX::XMFLOAT4 color;
	} pmc;

	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	float width;
	float height;
};
