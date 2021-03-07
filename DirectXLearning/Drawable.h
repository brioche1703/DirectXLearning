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
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Draw(Graphics& gfx) const noxnd;
	virtual void Update(float dt) noexcept {};

protected:
	template<class T>
	T* QueryBindable() noexcept {
		for (auto& pb : binds) {
			if (auto pt = dynamic_cast<T*>(pb.get())) {
				return pt;
			}
		}
		return nullptr;
	}

	void AddBind(std::unique_ptr<Bind::Bindable> bind) noxnd;
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> iBuffer) noexcept;

private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> binds;
};
