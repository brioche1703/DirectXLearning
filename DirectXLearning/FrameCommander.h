#pragma once

#include "Graphics.h"
#include "BindableCommon.h"
#include "Pass.h"
#include "PerfLog.h"

#include <array>

class FrameCommander {
public:
	void Accept(Job job, size_t target) noexcept {
		passes[target].Accept(job);
	}

	void Execute(Graphics& gfx) const noxnd {
		using namespace Bind;

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
	}

	void Reset() noexcept {
		for (auto& p : passes) {
			p.Reset();
		}
	}

private:
	std::array<Pass, 3> passes;
};
