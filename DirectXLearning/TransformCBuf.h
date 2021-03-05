#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include "Bindable.h"
#include "ConstantBuffers.h"

#include <DirectXMath.h>

class TransformCBuf : public Bindable {
private:
	struct Transforms {
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX modelViewProj;
	};

public:
	TransformCBuf(Graphics& gfx, Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pvcBuffer;
	const Drawable& parent;
};
