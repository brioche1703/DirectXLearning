#pragma once

#include "FullScreenPass.h"
#include "ConstantBuffersEX.h"

namespace Rgph {
class VerticalBlurPass : public FullScreenPass {
public:
	VerticalBlurPass(std::string name, Graphics& gfx);
	void Execute(Graphics& gfx) const noxnd override;

private:
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};}
