#pragma once

#include "RenderQueuePass.h"
#include "Stencil.h"
#include "Sink.h"
#include "Source.h"
#include "Camera.h"
#include "ShadowCameraCBuf.h"
#include "ShadowSampler.h"
#include "Sampler.h"
#include "Viewport.h"

namespace Rgph {
	class LambertianPass : public RenderQueuePass {
	public:
		LambertianPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name)),
			pShadowCBuf(std::make_shared<Bind::ShadowCameraCBuf>(gfx))
		{
			using namespace Bind;
			AddBind(pShadowCBuf);
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			AddBindSink<Bind::Bindable>("shadowMap");
			AddBind(std::make_shared<Bind::ShadowSampler>(gfx));
			AddBindSink<Bind::Bindable>("shadowControl");
			AddBindSink<Bind::Bindable>("gammaCorrectionControl");
			AddBindSink<Bind::Bindable>("shadowSampler");
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}

		void BindMainCamera(const Camera& cam) noexcept {
			pMainCamera = &cam;
		}

		void BindShadowCamera(const Camera& cam) noexcept {
			pShadowCBuf->SetCamera(&cam);
		}

		void Execute(Graphics& gfx) const noxnd override {
			assert(pMainCamera);
			pShadowCBuf->Update(gfx);
			pMainCamera->BindToGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}

	private:
		std::shared_ptr<Bind::ShadowCameraCBuf> pShadowCBuf;
		const Camera* pMainCamera = nullptr;
	};
}
