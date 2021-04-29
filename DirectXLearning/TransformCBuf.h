#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include "Bindable.h"
#include "ConstantBuffers.h"

#include <DirectXMath.h>

namespace Bind {
	class TransformCBuf : public CloningBindable {
	protected:
		struct Transforms {
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};

	public:
		TransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noxnd override;

		void InitializeParentReference(const Drawable& parent) noexcept override;

		std::unique_ptr<CloningBindable> Clone() const noexcept override;

	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		Transforms GetTransforms(Graphics& gfx) noxnd;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pvcBuffer;
		const Drawable* pParent = nullptr;
	};
}
