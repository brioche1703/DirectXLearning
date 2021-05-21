#include "SolidSphere.h"
#include "Sphere.h"
#include "BindableCommon.h"
#include "Stencil.h"
#include "Channels.h"

#include <memory>

SolidSphere::SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color)
	:
	color(color)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	
	const auto geometryTag = "$sphere." + std::to_string(radius);

	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid(Chan::main);
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

		Dcb::RawLayout lay;
		lay.Add<Dcb::Float3>("color");
		auto buf = Dcb::Buffer(std::move(lay));
		buf["color"] = color;
		only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

		only.AddBindable(std::make_shared<TransformCBuf>(gfx));

		only.AddBindable(Rasterizer::Resolve(gfx, false));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
