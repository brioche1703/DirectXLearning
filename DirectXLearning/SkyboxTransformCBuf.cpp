#include "SkyboxTransformCBuf.h"

#include <DirectXMath.h>

namespace Bind {
	SkyboxTransformCBuf::SkyboxTransformCBuf(Graphics& gfx, UINT slot) 
		:
		pVcbuf(std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot))
	{}

	void SkyboxTransformCBuf::Bind(Graphics& gfx) noxnd {
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(UpdateBindImpl(gfx, GetTransforms(gfx)));
	}

	void SkyboxTransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd {
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	SkyboxTransformCBuf::Transforms SkyboxTransformCBuf::GetTransforms(Graphics& gfx) noxnd {
		return {
			DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection())
		};
	}
}
