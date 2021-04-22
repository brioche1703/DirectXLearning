#pragma once

#include "FullScreenPass.h"
#include "ConstantBuffersEX.h"

namespace Bind {
	class Bindable;
}

class HorizontalBlurPass : public FullScreenPass {
public:
	HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight);
	void Execute(Graphics& gfx) const noexcept override;

private:
	std::shared_ptr<Bind::Bindable> blurScratchIn;
	std::shared_ptr<Bind::Bindable> control;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};
