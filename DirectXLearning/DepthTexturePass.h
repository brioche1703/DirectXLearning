#pragma once

#include "ViewportPass.h"
#include "BindableCommon.h"

class Graphics;

namespace Rgph {
	class DepthTexturePass : public ViewportPass {
	public:
		DepthTexturePass(Graphics& gfx, std::string name)
			:
			ViewportPass(gfx, std::move(name), Bind::Viewport(gfx, 100, 600, 20.0f, 20.0f))
		{
			using namespace Bind;
			AddBind(PixelShader::Resolve(gfx, "DepthTexture_PS.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
			AddBind(Blender::Resolve(gfx, false));

			AddBindSink<Bind::Bindable>("shadowMapTexture");

			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
		}

		void Execute(Graphics& gfx) const noxnd {
			ViewportPass::Execute(gfx);
		}
	};
}
