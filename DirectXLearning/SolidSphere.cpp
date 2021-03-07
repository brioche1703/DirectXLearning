#include "SolidSphere.h"
#include "Sphere.h"
#include "BindableCommon.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;
	namespace dx = DirectX;

	if (!IsStaticInitialized()) {
		struct Vertex {
			dx::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform(dx::XMMatrixScaling(radius, radius, radius));

		AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));


		struct PSColorConstant {
			dx::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
			float padding;
		} psC;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, psC));
	
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else {
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept {

}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
