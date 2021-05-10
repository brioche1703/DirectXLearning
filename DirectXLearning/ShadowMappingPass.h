#pragma once

#include "RenderQueuePass.h"
#include "BindableCommon.h"
#include "Camera.h"


class Graphics;

namespace Rgph {
	class ShadowMappingPass : public RenderQueuePass {
	public:
		void BindShadowCamera(const Camera& cam) noexcept {
			pShadowCamera = &cam;
		}

		ShadowMappingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			pDepthCube = std::make_shared<DepthCubeTexture>(gfx, size, 3);
			AddBind(VertexShader::Resolve(gfx, "Shadow_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(Blender::Resolve(gfx, false));
			AddBind(std::make_shared<Viewport>(gfx, (float)size, (float)size));
			AddBindSink<Bind::Bindable>("shadowRasterizer");
			RegisterSource(DirectBindableSource<Bind::DepthCubeTexture>::Make("map", pDepthCube));

			DirectX::XMStoreFloat4x4(
				&projection,
				DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f, 1.0f, 60.0f)
			);

			DirectX::XMStoreFloat3(&cameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[2], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[3], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			SetDepthBuffer(pDepthCube->GetDepthBuffer(0));
		}

		void Execute(Graphics& gfx) const noxnd override {
			using namespace DirectX;

			const auto shadowCameraPos = pShadowCamera->GetPos();
			const auto pos = XMLoadFloat3(&shadowCameraPos);

			gfx.SetProjection(XMLoadFloat4x4(&projection));

			for (size_t i = 0; i < 6; i++) {
				auto d = pDepthCube->GetDepthBuffer(i);
				d->Clear(gfx);
				SetDepthBuffer(std::move(d));
				const auto lookAt = pos + XMLoadFloat3(&cameraDirections[i]);
				gfx.SetCamera(XMMatrixLookAtLH(pos, lookAt, XMLoadFloat3(&cameraUps[i])));

				RenderQueuePass::Execute(gfx);
			}
		}

		//void DumpShadowMap(Graphics& gfx, const std::string& path) const {
		//	depthStencil->ToSurface(gfx).Save(path);
		//}

	private:
		void SetDepthBuffer(std::shared_ptr<Bind::DepthStencil> ds) const {
			const_cast<ShadowMappingPass*>(this)->depthStencil = std::move(ds);
		}

	private:
		static constexpr UINT size = 1000;
		const Camera* pShadowCamera = nullptr;
		std::shared_ptr<Bind::DepthCubeTexture>pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}
