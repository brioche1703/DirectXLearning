#pragma once

#include "RenderQueuePass.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "Source.h"

namespace Rgph {
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
			RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOutput", renderTarget));
		}

		void Execute(Graphics& gfx) const noxnd override {
			renderTarget->Clear(gfx);
			RenderQueuePass::Execute(gfx);
		}
	};
}
