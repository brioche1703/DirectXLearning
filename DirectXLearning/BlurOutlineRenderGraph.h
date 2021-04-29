#pragma once

#include "RenderGraph.h"
#include "ConstantBuffersEX.h"

#include <memory>

class Camera;

namespace Rgph {
	class BlurOutlineRenderGraph : public RenderGraph {
	public:
		BlurOutlineRenderGraph(Graphics& gfx);

		void RenderWidgets(Graphics& gfx);
		void BindMainCamera(Camera& cam);
		void BindShadowCamera(Camera& cam);
		void DumpShadowMap(Graphics& gfx, const std::string& path);

	private:
		void SetKernelGauss(int radius, float sigma) noxnd;
		void SetKernelBox(int radius) noxnd;

		enum class KernelType {
			Gauss,
			Box
		} kernelType = KernelType::Gauss;

	private:
		static constexpr int maxRadius = 15;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
	};
}
