#pragma once

#include "Drawable.h"
#include "ConditionalNoexcept.h"

template<class T>
class DrawableBase : public Drawable {
protected:
	bool IsStaticInitialized() const noexcept {
		return !staticBinds.empty();
	}

	void AddStaticBind(std::unique_ptr<Bind::Bindable> bind) noxnd {
		assert("Must use AddStaticIndexBuffer to bind an index buffer!" && typeid(*bind) != typeid(Bind::IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> iBuffer) noxnd {
		assert("You are adding a second index buffer!" && pIndexBuffer == nullptr);
		pIndexBuffer = iBuffer.get();
		staticBinds.push_back(std::move(iBuffer));
	}

	void SetIndexFromStatic() noxnd {
		assert("You are adding a second index buffer!" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds) {
			if (const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get())) {
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}
	
private:
	const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept {
		return staticBinds;
	}

private:
	static std::vector<std::unique_ptr<Bind::Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bind::Bindable>> DrawableBase<T>::staticBinds;
