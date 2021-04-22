#pragma once

#include "BindingPass.h"

#include <memory>
#include <string>

namespace Bind {
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class InputLayout;
}

class FullScreenPass : public BindingPass {
public:
	FullScreenPass(const std::string name, Graphics& gfx) noxnd;
	void Execute(Graphics& gfx) const noexcept override;
};
