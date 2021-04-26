#pragma once

#include "BindingPass.h"

#include <memory>
#include <string>

namespace Rgph {
	class FullScreenPass : public BindingPass {
	public:
		FullScreenPass(const std::string name, Graphics& gfx) noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	};
}
