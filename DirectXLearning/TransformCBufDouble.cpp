#include "TransformCBufDouble.h"

namespace Bind {
	TransformCBufDouble::TransformCBufDouble(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP) 
		:
		TransformCBuf(gfx, parent, slotV)
	{
		if (!pPcBuf) {
			pPcBuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}
	void TransformCBufDouble::Bind(Graphics& gfx) noexcept {
		const auto tf = GetTransforms(gfx);
		TransformCBuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}
	void TransformCBufDouble::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept {
		pPcBuf->Update(gfx, tf);
		pPcBuf->Bind(gfx);
	}

	std::unique_ptr<PixelConstantBuffer<TransformCBuf::Transforms>> TransformCBufDouble::pPcBuf;
}