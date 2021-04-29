#include "TransformCBuf.h"

namespace Bind {
	TransformCBuf::TransformCBuf(Graphics& gfx, UINT slot)
	{
		if (!pvcBuffer) {
			pvcBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noxnd {
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(UpdateBindImpl(gfx, GetTransforms(gfx)));
	}

	void TransformCBuf::InitializeParentReference(const Drawable& parent) noexcept {
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCBuf::Clone() const noexcept {
		return std::make_unique<TransformCBuf>(*this);
	}

	void TransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd {		
		assert(pParent != nullptr);
		pvcBuffer->Update(gfx, tf);
		pvcBuffer->Bind(gfx);
	}

	TransformCBuf::Transforms TransformCBuf::GetTransforms(Graphics& gfx) noxnd {
		assert(pParent != nullptr);
		const auto model = pParent->GetTransformXM();
		const auto modelView = pParent->GetTransformXM() * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection())
		};

	}

	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::pvcBuffer;
}
