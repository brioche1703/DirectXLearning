#pragma once

#include "FullScreenPass.h"
#include "ConstantBuffersEX.h"

namespace Bind {
	class Bindable;
}

namespace Rgph {
class HorizontalBlurPass : public FullScreenPass {
public:
	HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight);
	void Execute(Graphics& gfx) const noxnd override;

private:
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};}
