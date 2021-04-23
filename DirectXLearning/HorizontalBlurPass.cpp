#include "HorizontalBlurPass.h"

#include "BindableCommon.h"
#include "Source.h"
#include "Sink.h"

namespace Rgph {
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		:
		FullScreenPass(std::move(name), gfx)
	{
		AddBind(Bind::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Bind::Blender::Resolve(gfx, false));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::CachingPixelConstantBufferEx>("control");
		RegisterSink(DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make("direction", direction));

		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOutput", renderTarget));
	}

	void HorizontalBlurPass::Execute(Graphics& gfx) const noxnd {
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		direction->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}
