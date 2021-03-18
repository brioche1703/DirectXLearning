#pragma once

#include "Graphics.h"
#include "ConditionalNoexcept.h"

#include <DirectXMath.h>
#include <memory>

namespace Bind {
	class Bindable;
	class IndexBuffer;
}

class Drawable {

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;

	template<class T>
	T* QueryBindable() noexcept {
		for (auto& pb : binds) {
			if (auto pt = dynamic_cast<T*>(pb.get())) {
				return pt;
			}
		}
		return nullptr;
	}

protected:
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noxnd;

private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};
