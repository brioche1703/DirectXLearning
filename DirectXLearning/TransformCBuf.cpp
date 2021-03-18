#include "TransformCBuf.h"

namespace Bind {
	TransformCBuf::TransformCBuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		parent(parent)
	{
		if (!pvcBuffer) {
			pvcBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept {
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept {		
		pvcBuffer->Update(gfx, tf);
		pvcBuffer->Bind(gfx);
	}

	TransformCBuf::Transforms TransformCBuf::GetTransforms(Graphics& gfx) noexcept {
		const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection())
		};

	}

	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::pvcBuffer;
}
