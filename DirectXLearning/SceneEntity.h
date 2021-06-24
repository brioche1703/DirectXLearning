#pragma once

#include <string>

namespace Rgph {
	class RenderGraph;
}

class SceneEntity {
public:
	virtual ~SceneEntity() = default;

	virtual void LinkTechniques(Rgph::RenderGraph& rg) = 0;
	virtual void Submit(size_t channels) const noxnd = 0;
	virtual const std::string& GetName() const noexcept = 0;
	virtual void OnImguiRender(bool enable) noexcept = 0;
};
