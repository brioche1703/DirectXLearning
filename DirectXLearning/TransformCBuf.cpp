#include "TransformCBuf.h"

TransformCBuf::TransformCBuf(Graphics& gfx, Drawable& parent)
	: 
	parent(parent) 
{
	if (!pvcBuffer) {
		pvcBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformCBuf::Bind(Graphics& gfx) noexcept {
	pvcBuffer->Update(
		gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()));
	pvcBuffer->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuf::pvcBuffer;

