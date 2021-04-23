#pragma once

#include "RenderGraph.h"
#include "ConstantBuffersEX.h"

#include <memory>

namespace Rgph {
	class BlurOutlineRenderGraph : public RenderGraph {
	public:
		BlurOutlineRenderGraph(Graphics& gfx);

	private:
		void SetKernelGauss(int radius, float sigma) noxnd;

	private:
		static constexpr int maxRadius = 15;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurControl;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}
