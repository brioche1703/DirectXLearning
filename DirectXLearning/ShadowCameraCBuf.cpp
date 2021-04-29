#include "ShadowCameraCBuf.h"
#include "Camera.h"


namespace Bind {
	ShadowCameraCBuf::ShadowCameraCBuf(Graphics& gfx, UINT slot)
		:
		pVcbuf{std::make_unique<VertexConstantBuffer<Transform>>(gfx, slot)}
	{}

	void ShadowCameraCBuf::Bind(Graphics& gfx) noxnd {
		pVcbuf->Bind(gfx);
	}
	
	void ShadowCameraCBuf::Update(Graphics& gfx) {
		namespace dx = DirectX;
		const Transform t = {
			dx::XMMatrixTranspose(
				pCamera->GetMatrix() * pCamera->GetProjection()
			)
		};
		pVcbuf->Update(gfx, t);
	}

	void ShadowCameraCBuf::SetCamera(const Camera* pCam) noexcept {
		pCamera = pCam;
	}
}
