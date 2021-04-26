#include "FullScreenPass.h"
#include "BindableCommon.h"

namespace dx = DirectX;

namespace Rgph {
	FullScreenPass::FullScreenPass(const std::string name, Graphics& gfx) noxnd
		:
	BindingPass(std::move(name))
	{
		dxLearning::VertexLayout lay;
		lay.Append(dxLearning::VertexLayout::Position2D);
		dxLearning::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,  1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{  1,  1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, -1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{  1, -1 });

		AddBind(Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));

		std::vector<unsigned short> indices{ 0, 1, 2, 1, 3, 2 };
		AddBind(Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));

		auto vs = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");

		AddBind(Bind::InputLayout::Resolve(gfx, lay, *vs));
		AddBind(std::move(vs));
		AddBind(Bind::Topology::Resolve(gfx));
		AddBind(Bind::Rasterizer::Resolve(gfx, false));
	}

	void FullScreenPass::Execute(Graphics& gfx) const noxnd {
		BindAll(gfx);

		gfx.DrawIndexed(6u);
	}
}
