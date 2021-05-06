#pragma once

#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Drawable.h"

class Camera;

namespace Bind {
	class SkyboxTransformCBuf : public Bindable {
	protected:
		struct Transforms {
			DirectX::XMMATRIX viewProj;
		};

	public:
		SkyboxTransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noxnd override;

	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		Transforms GetTransforms(Graphics& gfx) noxnd;

	private:
		std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	};
}
