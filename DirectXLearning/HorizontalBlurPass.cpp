#include "HorizontalBlurPass.h"

#include "BindableCommon.h"
#include "Source.h"
#include "Sink.h"

using namespace Bind;

namespace Rgph {
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		:
		FullScreenPass(std::move(name), gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blender::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Point, true));

		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", direction));

		renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectBindableSource<RenderTarget>::Make("scratchOutput", renderTarget));
	}

	void HorizontalBlurPass::Execute(Graphics& gfx) const noxnd {
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}
