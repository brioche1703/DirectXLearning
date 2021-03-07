#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd {
	for (auto& b : binds) {
		b->Bind(gfx);
	}
	for (auto& b : GetStaticBinds()) {
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noxnd {
	assert("Must use AddIndexBuffer to bind index buffer!" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> iBuffer) noexcept {
	assert("You are adding a second index buffer!" && pIndexBuffer == nullptr);
	pIndexBuffer = iBuffer.get();
	binds.push_back(std::move(iBuffer));
}
