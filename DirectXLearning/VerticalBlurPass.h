#pragma once

#include "FullScreenPass.h"
#include "ConstantBuffersEX.h"

class VerticalBlurPass : public FullScreenPass {
public:
	VerticalBlurPass(std::string name, Graphics& gfx);
	void Execute(Graphics& gfx) const noexcept override;

private:
	std::shared_ptr<Bind::Bindable> blurScratchIn;
	std::shared_ptr<Bind::Bindable> control;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};
