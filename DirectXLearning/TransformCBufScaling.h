#pragma once

#include "TransformCBuf.h"
#include "DynamicConstant.h"

namespace Bind {
	class TransformCBufScaling : public TransformCBuf {
	public:
		TransformCBufScaling(Graphics& gfx, float scale);
		void Accept(TechniqueProbe& probe) override;
		void Bind(Graphics& gfx) noxnd override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;

	private:
		static Dcb::RawLayout MakeLayout();

	private:
		Dcb::Buffer buf;

	};
}
