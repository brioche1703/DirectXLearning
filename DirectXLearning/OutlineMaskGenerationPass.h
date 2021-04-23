#pragma once

#include "RenderQueuePass.h"
#include "VertexShader.h"
#include "NullPixelShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

namespace Rgph {
	class OutlineMaskGenerationPass : public RenderQueuePass {
	public:
		OutlineMaskGenerationPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
			AddBind(Rasterizer::Resolve(gfx, false));
		}
	};
}
