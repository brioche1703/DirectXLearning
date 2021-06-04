#pragma once

#include "ViewportPass.h"
#include "BindableCommon.h"

class Graphics;

namespace Rgph {
	class DepthTexturePass : public ShadowMapViewportPass {
	public:
		DepthTexturePass(Graphics& gfx, std::string name)
			:
			ShadowMapViewportPass(gfx, std::move(name))
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
			ShadowMapViewportPass::Execute(gfx);
		}
	};
}
