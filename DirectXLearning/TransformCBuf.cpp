#include "TransformCBuf.h"

namespace Bind {
	TransformCBuf::TransformCBuf(Graphics& gfx, Drawable& parent, UINT slot)
		:
		parent(parent)
	{
		if (!pvcBuffer) {
			pvcBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept {
		const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		const Transforms t = {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection())
		};

		pvcBuffer->Update(gfx, t);

		pvcBuffer->Bind(gfx);
	}

	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::pvcBuffer;
}
