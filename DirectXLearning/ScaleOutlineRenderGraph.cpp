#include "ScaleOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"

ScaleOutlineRenderGraph::ScaleOutlineRenderGraph(Graphics& gfx) 
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
		{
			auto pass = std::make_unique<OutlineDrawingPass>(gfx, "outlineDraw");
			pass->SetInputResource("renderTarget", "lambertian.renderTarget");
			pass->SetInputResource("depthStencil", "outlineMask.depthStencil");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
		Finalize();
}
