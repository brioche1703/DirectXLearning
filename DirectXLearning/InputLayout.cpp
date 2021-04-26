#include "InputLayout.h"
#include "BindableCodex.h"
#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

namespace Bind {
	InputLayout::InputLayout(Graphics& gfx, const dxLearning::VertexLayout layout_in, const VertexShader& vs)
		: 
		layout(std::move(layout_in))
	{
		INFOMAN(gfx);

		const auto d3dLayout = layout.GetD3DLayout();
		const auto pBytecode = vs.GetBytecode();

		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			d3dLayout.data(),
			(UINT)d3dLayout.size(),
			pBytecode->GetBufferPointer(),
			pBytecode->GetBufferSize(),
			&pInputLayout));
	}

	const dxLearning::VertexLayout InputLayout::GetLayout() const noexcept {
		return layout;
	}

	void InputLayout::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(
		Graphics& gfx, 
		const dxLearning::VertexLayout& layout, 
		const VertexShader& vs) {
		return Codex::Resolve<InputLayout>(gfx, layout, vs);
	}

	std::string InputLayout::GenerateUID(
		const dxLearning::VertexLayout& layout, 
		const VertexShader& vs) {
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}

	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
	}
}
