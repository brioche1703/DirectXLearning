#pragma once

#include "RenderQueuePass.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class OutlineDrawingPass : public RenderQueuePass {
public:
	OutlineDrawingPass(Graphics& gfx, std::string name) 
		:
		RenderQueuePass(std::move(name))
	{
		using namespace Bind;
		RegisterInput(BufferInput<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", depthStencil));
		RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", depthStencil));

		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Rasterizer::Resolve(gfx, false));
	}
};
