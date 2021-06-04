#include "ViewportPass.h"

#include "BindableCommon.h"

Rgph::ViewportPass::ViewportPass(Graphics& gfx, std::string name, Bind::Viewport vp) noxnd
	:
	BindingPass(std::move(name))
{
	namespace dx = DirectX;
	using namespace Bind;
	dxLearning::VertexLayout lay;
	lay.Append(dxLearning::VertexLayout::Position2D);
	dxLearning::VertexBuffer bufFull{ lay };
	bufFull.EmplaceBack(dx::XMFLOAT2{ -1,  1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ 1,  1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ -1, -1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ 1, -1 });

	AddBind(VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));

	std::vector<unsigned short> indices{ 0, 1, 2, 1, 3, 2 };
	AddBind(IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));

	auto vs = VertexShader::Resolve(gfx, "Fullscreen_VS.cso");

	AddBind(InputLayout::Resolve(gfx, lay, *vs));
	AddBind(std::move(vs));
	AddBind(Topology::Resolve(gfx));
	AddBind(Rasterizer::Resolve(gfx, false));
	AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
	AddBind(std::make_unique<Viewport>(vp));
}

void Rgph::ViewportPass::Execute(Graphics& gfx) const noxnd {
		BindAll(gfx);
		gfx.DrawIndexed(6u);
}

Rgph::ShadowMapViewportPass::ShadowMapViewportPass(Graphics& gfx, std::string name) noxnd
	:
	ViewportPass(gfx, name, Bind::Viewport{ gfx, (gfx.GetHeight() - 100.0f) / 6.0f, gfx.GetHeight() - 100.0f, 20.0f, 20.0f })
{}

void Rgph::ShadowMapViewportPass::Execute(Graphics& gfx) const noxnd {
		BindAll(gfx);
		auto vp = std::make_unique<Bind::Viewport>(gfx, (gfx.GetHeight() - 100.0f) / 6.0f, gfx.GetHeight() - 100.0f, 20.0f, 20.0f);
		vp->Bind(gfx);
		gfx.DrawIndexed(6u);
}
