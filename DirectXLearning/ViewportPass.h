#pragma once

#include "BindingPass.h"
#include "Viewport.h"

#include <memory>
#include <string>

namespace Bind {
	class Viewport;
}

namespace Rgph {
	class ViewportPass : public BindingPass {
	public:
		ViewportPass(Graphics& gfx, std::string name, Bind::Viewport vp)noxnd;
		void Execute(Graphics& gfx) const noxnd override;
	};
}

