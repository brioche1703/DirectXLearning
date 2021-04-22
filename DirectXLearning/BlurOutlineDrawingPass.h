#pragma once

#include "RenderQueuePass.h"
#include "BindableCommon.h"

class BlurOutlineDrawingPass : public RenderQueuePass {
public:
	BlurOutlineDrawingPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight) 
		:
		RenderQueuePass(std::move(name))
	{
		using namespace Bind;
		renderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Blender::Resolve(gfx, false));
		RegisterOutput(ImmutableOutput<Bind::RenderTarget>::Make("scratchOutput", renderTarget));
	}
};
