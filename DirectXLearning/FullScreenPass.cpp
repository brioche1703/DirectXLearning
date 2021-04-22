#include "FullScreenPass.h"
#include "BindableCommon.h"

namespace dx = DirectX;

FullScreenPass::FullScreenPass(const std::string name, Graphics& gfx) noxnd
	:
BindingPass(name)
{
	if (!indexBuffer) {
		dxLearning::VertexLayout lay;
		lay.Append(dxLearning::VertexLayout::Position2D);
		dxLearning::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,  1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,  1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, -1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,  1 });

		vertexBuffer = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));

		std::vector<unsigned short> indices{ 0, 1, 2, 1, 3, 2 };
		indexBuffer = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

		vertexShader = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		inputLayout = Bind::InputLayout::Resolve(gfx, lay, vertexShader->GetBytecode());
	}
}

void FullScreenPass::Execute(Graphics& gfx) const noexcept {
	BindAll(gfx);
	vertexBuffer->Bind(gfx);
	indexBuffer->Bind(gfx);
	vertexShader->Bind(gfx);
	inputLayout->Bind(gfx);

	gfx.DrawIndexed(indexBuffer->GetCount());
}

std::shared_ptr<Bind::VertexBuffer> FullScreenPass::vertexBuffer;
std::shared_ptr<Bind::VertexShader> FullScreenPass::vertexShader;
std::shared_ptr<Bind::IndexBuffer> FullScreenPass::indexBuffer;
std::shared_ptr<Bind::InputLayout> FullScreenPass::inputLayout;
