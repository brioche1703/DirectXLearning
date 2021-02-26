#pragma once

#include "Drawable.h"
#include "Bindable.h"
#include <memory>

template<class T>
class DrawableBase : public Drawable {
protected:
	bool IsStaticInitialized() const noexcept {
		return !staticBinds.empty();
	}

	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG) {
		assert("Must use AddStaticIndexBuffer to bind an index buffer!" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> iBuffer) noexcept(!IS_DEBUG) {
		assert("You are adding a second index buffer!" && pIndexBuffer == nullptr);
		pIndexBuffer = iBuffer.get();
		staticBinds.push_back(std::move(iBuffer));
	}

	void SetIndexFromStatic() noexcept(!IS_DEBUG) {
		assert("You are adding a second index buffer!" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds) {
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get())) {
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}
	
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept {
		return staticBinds;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;
