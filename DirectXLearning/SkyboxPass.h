#pragma once

#include "BindingPass.h"
#include "BindableCommon.h"
#include "Cube.h"
#include "Camera.h"

class Graphics;

namespace Rgph {
	class SkyboxPass : public BindingPass {
	public:
		SkyboxPass(Graphics& gfx, std::string name) 
			:
			BindingPass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(std::make_shared<CubeTexture>(gfx, "src\\images\\SpaceBox"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
			AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));
			AddBind(Rasterizer::Resolve(gfx, true));
			AddBind(std::make_shared<SkyboxTransformCBuf>(gfx));
			AddBind(PixelShader::Resolve(gfx, "Skybox_PS.cso"));
			AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			
			{
				auto pvs = Bind::VertexShader::Resolve(gfx, "Skybox_VS.cso");
				auto model = Cube::Make();
				model.Transform(DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f));
				const auto geometryTag = "$cube_map";
				AddBind(VertexBuffer::Resolve(gfx, geometryTag, std::move(model.vertices)));
				count = (UINT)model.indices.size();
				AddBind(IndexBuffer::Resolve(gfx, geometryTag, std::move(model.indices)));
				AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
				AddBind(std::move(pvs));
			}
			
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
		}

		void BindMainCamera(const Camera& cam) noexcept {
			pMainCamera = &cam;
		}

		void Execute(Graphics& gfx) const noxnd override {
			assert(pMainCamera);
			pMainCamera->BindToGraphics(gfx);
			BindAll(gfx);
			gfx.DrawIndexed(count);
		}

	private:
		UINT count;
		const Camera* pMainCamera = nullptr;
	};
}
