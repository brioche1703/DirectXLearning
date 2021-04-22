#include "BlurOutlineRenderGraph.h"

#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineMaskGenerationPass.h"
#include "BlurOutlineDrawingPass.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "ConstantBuffersEX.h"
#include "MathsUtils.h"

BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
	:
	RenderGraph(gfx)
{
	{
		auto pass = std::make_unique<BufferClearPass>("clear");
		pass->SetInputResource("renderTarget", "$.backbuffer");
		pass->SetInputResource("depthStencil", "$.masterDepth");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
		pass->SetInputResource("renderTarget", "clear.renderTarget");
		pass->SetInputResource("depthStencil", "clear.depthStencil");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
		pass->SetInputResource("depthStencil", "lambertian.depthStencil");
		AppendPass(std::move(pass));
	}

	// Set Blur Constant Buffer
	{
		{
			Dcb::RawLayout lay;
			lay.Add<Dcb::Integer>("nTaps");
			lay.Add<Dcb::Array>("coefficients");
			lay["coefficients"].Set<Dcb::Float>(maxRadius * 2 + 1);
			Dcb::Buffer buf{ std::move(lay) };

			blurControl = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0);
			SetKernelGauss(radius, sigma);
			AddGlobalSource(ImmutableOutput<Bind::CachingPixelConstantBufferEx>::Make("blurControl", blurControl));
		}
		{
			Dcb::RawLayout lay;
			lay.Add<Dcb::Bool>("isHorizontal");
			Dcb::Buffer buf{ std::move(lay) };
			blurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1);
			AddGlobalSource(ImmutableOutput<Bind::CachingPixelConstantBufferEx>::Make("blurDirection", blurDirection));
		}
	}

	{
		auto pass = std::make_unique<BlurOutlineDrawingPass>(gfx, "outlineDrawing", gfx.GetWidth(), gfx.GetHeight());
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
		pass->SetInputResource("scratchIn", "outlineDraw.scratchOut");
		pass->SetInputResource("control", "$.blurControl");
		pass->SetInputResource("direction", "$.blurDirection");
		AppendPass(std::move(pass));
	}
	{
		auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
		pass->SetInputResource("renderTarget", "lambertian.renderTarget");
		pass->SetInputResource("depthStencil", "outlineMask.depthStencil");
		pass->SetInputResource("scratchIn", "horizontal.scratchOut");
		pass->SetInputResource("control", "$.blurControl");
		pass->SetInputResource("direction", "$.blurDirection");
		AppendPass(std::move(pass));
	}
	SetSinkTarget("backbuffer", "vertical.renderTarget");
	Finalize();
}

void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noxnd
{
	assert(radius <= maxRadius);
	auto k = blurControl->GetBuffer();
	const int nTaps = radius * 2 + 1;
	k["nTaps"] = nTaps;
	float sum = 0.0f;
	for (int i = 0; i < nTaps; i++)
	{
		const auto x = float(i - radius);
		const auto g = gauss(x, sigma);
		sum += g;
		k["coefficients"][i] = g;
	}
	for (int i = 0; i < nTaps; i++)
	{
		k["coefficients"][i] = (float)k["coefficients"][i] / sum;
	}
	blurControl->SetBuffer(k);
}

