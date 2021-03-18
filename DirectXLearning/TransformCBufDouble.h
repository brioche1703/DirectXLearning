#pragma once

#include "TransformCBuf.h"

namespace Bind {
	class TransformCBufDouble : public TransformCBuf {
	public:
		TransformCBufDouble(Graphics& gfx, const Drawable& parent, UINT slotV = 0u, UINT slotP = 1u);
		void Bind(Graphics& gfx) noexcept override;

	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;

	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcBuf;
	};
}
