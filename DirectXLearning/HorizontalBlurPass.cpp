#include "HorizontalBlurPass.h"

#include "BindableCommon.h"

HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight) 
	:
	FullScreenPass(std::move(name), gfx)
{
	AddBind(Bind::PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
	AddBind(Bind::Blender::Resolve(gfx, false));

	RegisterInput(ImmutableInput<Bind::Bindable>::Make("control", control));
	RegisterInput(ImmutableInput<Bind::CachingPixelConstantBufferEx>::Make("direction", direction));
	RegisterInput(ImmutableInput<Bind::Bindable>::Make("scratchIn", blurScratchIn));

	renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
	RegisterOutput(ImmutableOutput<Bind::RenderTarget>::Make("scratchOut", renderTarget));
}

void HorizontalBlurPass::Execute(Graphics& gfx) const noexcept {
	auto buf = direction->GetBuffer();
	buf["isHorizontal"] = true;
	direction->SetBuffer(buf);
	
	blurScratchIn->Bind(gfx);
	control->Bind(gfx);
	direction->Bind(gfx);
	FullScreenPass::Execute(gfx);
}
