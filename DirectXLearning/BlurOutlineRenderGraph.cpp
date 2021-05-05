#include "BlurOutlineRenderGraph.h"

#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineMaskGenerationPass.h"
#include "BlurOutlineDrawingPass.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "ConstantBuffersEX.h"
#include "MathsUtils.h"
#include "Source.h"
#include "Sink.h"
#include "WireframePass.h"
#include "ShadowMappingPass.h"
#include "ShadowSampler.h"
#include "ShadowRasterizer.h"

#include "external/imgui/imgui.h"

namespace Rgph {
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		// Shadow Rasterizer
		{
			shadowRasterizer = std::make_shared<Bind::ShadowRasterizer>(gfx, 10000, 0.0005f, 1.0f);
			AddGlobalSource(DirectBindableSource<Bind::ShadowRasterizer>::Make("shadowRasterizer", shadowRasterizer));
		}
		{
			auto pass = std::make_unique<ShadowMappingPass>(gfx, "shadowMap");
			pass->SetSinkLinkage("shadowRasterizer", "$.shadowRasterizer");
			AppendPass(std::move(pass));
		}
		// Shadow control buffer and sampler
		{
			Dcb::RawLayout lay;
			lay.Add<Dcb::Integer>("pcfLevel");
			lay.Add<Dcb::Float>("depthBias");
			lay.Add<Dcb::Bool>("hardwarePcf");
			Dcb::Buffer buf{ std::move(lay) };
			buf["pcfLevel"] = 0;
			buf["depthBias"] = 0.0005f;
			buf["hardwarePcf"] = true;
			shadowControl = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 2);
			AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("shadowControl", shadowControl));
		}
		{							 
			shadowSampler = std::make_shared<Bind::ShadowSampler>(gfx);
			AddGlobalSource(DirectBindableSource<Bind::ShadowSampler>::Make("shadowSampler", shadowSampler));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("shadowMap", "shadowMap.map");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			pass->SetSinkLinkage("shadowControl", "$.shadowControl");
			pass->SetSinkLinkage("shadowSampler", "$.shadowSampler");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
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

				blurKernel = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurKernel", blurKernel));
			}
			{
				Dcb::RawLayout lay;
				lay.Add<Dcb::Bool>("isHorizontal");
				Dcb::Buffer buf{ std::move(lay) };
				blurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurDirection", blurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawingPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOutput");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOutput");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<WireframePass>(gfx, "wireframe");
			pass->SetSinkLinkage("renderTarget", "vertical.renderTarget");
			pass->SetSinkLinkage("depthStencil", "vertical.depthStencil");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "wireframe.renderTarget");
		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noxnd {
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++) {
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++) {
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		blurKernel->SetBuffer(k);
	}

	void BlurOutlineRenderGraph::SetKernelBox(int radius) noxnd {
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		const float c = 1.0f / nTaps;
		for (int i = 0; i < nTaps; i++) {
			k["coefficients"][i] = c;
		}
		blurKernel->SetBuffer(k);
	}

	void BlurOutlineRenderGraph::RenderWindows(Graphics& gfx) {
		RenderKernelWindow(gfx);
		RenderShadowWindow(gfx);
	}

	void BlurOutlineRenderGraph::RenderShadowWindow(Graphics& gfx) {
		if (ImGui::Begin("Shadows")) {
			auto ctrl = shadowControl->GetBuffer();
			auto bilin = shadowSampler->GetBilinear();
			bool pcfChange = ImGui::SliderInt("PCF Level", &ctrl["pcfLevel"], 0, 4);
			bool biasChange = ImGui::SliderFloat("Post Bias", &ctrl["depthBias"], 0.0f, 1.0f, "%.6f", 3.6f);
			bool hardwarePcfChange = ImGui::Checkbox("Harware PCF", &ctrl["hardwarePcf"]);
			ImGui::Checkbox("Bilinear", &bilin);
			if (pcfChange || biasChange || hardwarePcfChange) {
				shadowControl->SetBuffer(ctrl);
			}
			shadowSampler->SetHardwarePcf(ctrl["hardwarePcf"]);
			shadowSampler->SetBilinear(bilin);

			{
				auto bias = shadowRasterizer->GetDepthBias();
				auto slop = shadowRasterizer->GetSlopBias();
				auto clamp = shadowRasterizer->GetClamp();
				bool biasChanged = ImGui::SliderInt("Pre Bias", &bias, 0, 100000);
				bool slopChanged = ImGui::SliderFloat("Slope Bias", &slop, 0.0f, 100.0f, "%.4f", 4.0f);
				bool clampChanged = ImGui::SliderFloat("Clamp", &clamp, 0.0001f, 0.5f, "%.4f", 2.5f);
				if (biasChanged || slopChanged || clampChanged) {
					shadowRasterizer->ChangeDepthBiasParameters(gfx, bias, slop, clamp);
				}
			}
		}
		ImGui::End();
	}

	void BlurOutlineRenderGraph::RenderKernelWindow(Graphics& gfx) {
		if (ImGui::Begin("Kernel")) {
			bool filterChanged = false; {
				const char* items[] = { "Gauss","Box" };
				static const char* curItem = items[0];
				if (ImGui::BeginCombo("Filter Type", curItem)) {
					for (int n = 0; n < std::size(items); n++) {
						const bool isSelected = (curItem == items[n]);
						if (ImGui::Selectable(items[n], isSelected)) {
							filterChanged = true;
							curItem = items[n];
							if (curItem == items[0]) {
								kernelType = KernelType::Gauss;
							}
							else if (curItem == items[1]) {
								kernelType = KernelType::Box;
							}
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			bool radChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);
			bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
			if (radChange || sigChange || filterChanged) {
				if (kernelType == KernelType::Gauss) {
					SetKernelGauss(radius, sigma);
				}
				else if (kernelType == KernelType::Box) {
					SetKernelBox(radius);
				}
			}
		}
		ImGui::End();
	}

	void BlurOutlineRenderGraph::BindMainCamera(Camera& cam) {
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindMainCamera(cam);
	}

	void BlurOutlineRenderGraph::BindShadowCamera(Camera& cam) {
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).BindShadowCamera(cam);
		dynamic_cast<LambertianPass&>(FindPassByName("lambertian")).BindShadowCamera(cam);
	}

	void BlurOutlineRenderGraph::DumpShadowMap(Graphics& gfx, const std::string& path) {
		dynamic_cast<ShadowMappingPass&>(FindPassByName("shadowMap")).DumpShadowMap(gfx, path);
	}
}
