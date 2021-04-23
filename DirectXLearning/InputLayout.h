#pragma once

#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "Bindable.h"
#include "Vertex.h"

#include <memory>

namespace Bind {
	class InputLayout : public Bindable {
	public:
		InputLayout(Graphics& gfx, const dxLearning::VertexLayout layout, ID3DBlob* pVertexShaderBytecode);
		void Bind(Graphics& gfx) noxnd override;

		const dxLearning::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const dxLearning::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
		static std::string GenerateUID(const dxLearning::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
		std::string GetUID() const noexcept override;

	private:
		dxLearning::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
