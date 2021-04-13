#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include "Bindable.h"
#include "ConstantBuffers.h"

#include <DirectXMath.h>

namespace Bind {
	class TransformCBuf : public Bindable {
	protected:
		struct Transforms {
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};

	public:
		TransformCBuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;

		void InitializeParentReference(const Drawable& parent) noexcept override;

	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pvcBuffer;
		const Drawable* pParent = nullptr;
	};
}
