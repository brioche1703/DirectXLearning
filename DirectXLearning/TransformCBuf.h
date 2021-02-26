#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include "Bindable.h"
#include "ConstantBuffers.h"

#include <DirectXMath.h>

class TransformCBuf : public Bindable {
public:
	TransformCBuf(Graphics& gfx, Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pvcBuffer;
	const Drawable& parent;
};
