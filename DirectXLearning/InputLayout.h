#pragma once

#include "Bindable.h"
#include "Vertex.h"



namespace Bind {
	class VertexShader;

	class InputLayout : public Bindable {
	public:
		InputLayout(Graphics& gfx, const dxLearning::VertexLayout layout, const VertexShader& vs);
		void Bind(Graphics& gfx) noxnd override;

		const dxLearning::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const dxLearning::VertexLayout& layout, const VertexShader& vs);
		static std::string GenerateUID(const dxLearning::VertexLayout& layout, const VertexShader& vs);
		std::string GetUID() const noexcept override;

	private:
		std::string vertexShaderUID;
		dxLearning::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
