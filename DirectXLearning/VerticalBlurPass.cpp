#include "VerticalBlurPass.h"

#include "BindableCommon.h"

VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx) 
	:
	FullScreenPass(std::move(name), gfx)
{
	using namespace Bind;
	AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
	AddBind(Blender::Resolve(gfx, true));
	AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));

	RegisterInput(ImmutableInput<Bindable>::Make("scratchIn", blurScratchIn));
	RegisterInput(ImmutableInput<Bindable>::Make("control", control));
	RegisterInput(ImmutableInput<CachingPixelConstantBufferEx>::Make("direction", direction));
	RegisterInput(BufferInput<RenderTarget>::Make("renderTarget", renderTarget));
	RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", depthStencil));

	RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", renderTarget));
	RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", depthStencil));
}

void VerticalBlurPass::Execute(Graphics& gfx) const noexcept {
	auto buf = direction->GetBuffer();
	buf["horizontal"] = false;
	direction->SetBuffer(buf);

	control->Bind(gfx);
	direction->Bind(gfx);
	blurScratchIn->Bind(gfx);
	FullScreenPass::Execute(gfx);
}
