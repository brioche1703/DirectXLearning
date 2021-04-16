#pragma once

#include "Graphics.h"
#include "BindableCommon.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"
#include "RenderTarget.h"

#include <array>

class FrameCommander {
public:
	FrameCommander(Graphics& gfx)
		:
		ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
		rt(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
		namespace dx = DirectX;

		// Fullscreen geometry
		dxLearning::VertexLayout lay;
		lay.Append(dxLearning::VertexLayout::Position2D);
		dxLearning::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, 1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1, 1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, -1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1, -1 });
		pVbFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIbFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

		// Fullscreen shaders
		pPsFull = Bind::PixelShader::Resolve(gfx, "Funk_PS.cso");
		pVsFull = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		pLayoutFull = Bind::InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());
	}

	void Accept(Job job, size_t target) noexcept {
		passes[target].Accept(job);
	}

	void Execute(Graphics& gfx) const noxnd {
		using namespace Bind;

		ds.Clear(gfx);
		rt.Clear(gfx);
		rt.BindAsTarget(gfx, ds);

		// Main Phong lighting pass
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);

		// Outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);

		// Outline drawing pass
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		passes[2].Execute(gfx);

		// Fullscreen pass
		gfx.BindSwapBuffer();
		rt.BindAsTexture(gfx, 0);
		pVbFull->Bind(gfx);
		pIbFull->Bind(gfx);
		pVsFull->Bind(gfx);
		pPsFull->Bind(gfx);
		pLayoutFull->Bind(gfx);
		gfx.DrawIndexed(pIbFull->GetCount());
	}

	void Reset() noexcept {
		for (auto& p : passes) {
			p.Reset();
		}
	}

private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
	RenderTarget rt;

	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::PixelShader> pPsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
};
