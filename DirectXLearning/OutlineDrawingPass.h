#pragma once

#include "RenderQueuePass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph {
	class OutlineDrawingPass : public RenderQueuePass {
	public:
		OutlineDrawingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
			AddBind(Rasterizer::Resolve(gfx, false));
		}
	};
}
