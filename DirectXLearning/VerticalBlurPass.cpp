#include "VerticalBlurPass.h"

#include "BindableCommon.h"
#include "PixelShader.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph {
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx)
		:
		FullScreenPass(std::move(name), gfx)
	{
		using namespace Bind;
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blender::Resolve(gfx, true));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::CachingPixelConstantBufferEx>("kernel");

		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", direction));
		RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
	}

	void VerticalBlurPass::Execute(Graphics& gfx) const noxnd {
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}
