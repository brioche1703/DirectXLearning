#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"
#include "ConditionalNoexcept.h"

#include <string>
#include <memory>

class Drawable;
class TechniqueProbe;

namespace Bind {
	class Bindable : public GraphicsResource {
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual ~Bindable() = default;

		virtual void InitializeParentReference(const Drawable&) noexcept {}
		virtual void Accept(TechniqueProbe&) {}

		virtual std::string GetUID() const noexcept {
			assert(false);
			return "";
		}
	};

	class CloningBindable : public Bindable {
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}
