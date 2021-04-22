#pragma once

#include "RenderQueuePass.h"
#include "VertexShader.h"
#include "NullPixelShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class OutlineMaskGenerationPass : public RenderQueuePass {
public:
	OutlineMaskGenerationPass(Graphics& gfx, std::string name) 
		:
		RenderQueuePass(std::move(name))
	{
		using namespace Bind;
		RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", depthStencil));
		RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", depthStencil));

		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(NullPixelShader::Resolve(gfx));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
		AddBind(Rasterizer::Resolve(gfx, false));
	}
};
