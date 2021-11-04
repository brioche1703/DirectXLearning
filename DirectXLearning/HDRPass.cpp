#include "HDRPass.h"

#include "BindableCommon.h"
#include "PixelShader.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph {
	HDRPass::HDRPass(std::string name, Graphics& gfx)
		:
		FullScreenPass(std::move(name), gfx)
	{
		using namespace Bind;

		AddBind(PixelShader::Resolve(gfx, "HDR_PS.cso"));

		AddBindSink<RenderTarget>("hdrIn");
		RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));

		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
	}

	void HDRPass::Execute(Graphics& gfx) const noxnd {
		FullScreenPass::Execute(gfx);
	}
}
