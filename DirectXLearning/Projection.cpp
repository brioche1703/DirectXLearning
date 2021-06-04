#include "Projection.h"
#include "external/imgui/imgui.h"

Projection::Projection(Graphics& gfx, float width, float height, float nearZ, float farZ)
	:
	width(width),
	height(height),
	nearZ(nearZ),
	farZ(farZ),
	frustum(gfx, width, height, nearZ, farZ),
	homeWidth(width), homeHeight(height), homeNearZ(nearZ), homeFarZ(farZ),
	gfx(&gfx)
{}

DirectX::XMMATRIX Projection::GetMatrix() const {
	return DirectX::XMMatrixPerspectiveLH(1.0f, (float)gfx->GetHeight() / (float)gfx->GetWidth(), nearZ, farZ);
}

void Projection::RenderWidgets(Graphics& gfx) {
	bool dirty = false;
	const auto dcheck = [&dirty](bool d) { dirty = dirty || d; };

	ImGui::Text("Projection");
	dcheck(ImGui::SliderFloat("Width", &width, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("Height", &height, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("NearZ", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 10.0f));
	dcheck(ImGui::SliderFloat("FarZ", &farZ, nearZ + 0.01f, 400.0f, "%.2f", 10.0f));

	if (dirty) {
		frustum.SetVertices(gfx, width, height, nearZ, farZ);
	}
}

void Projection::SetPos(DirectX::XMFLOAT3 pos) {
	frustum.SetPos(pos);
}

void Projection::SetRotation(DirectX::XMFLOAT3 rot) {
	frustum.SetRotation(rot);
}

void Projection::Submit(size_t channel) const {
	frustum.Submit(channel);
}

void Projection::LinkTechniques(Rgph::RenderGraph& rg) {
	frustum.LinkTechniques(rg);
}

void Projection::Reset(Graphics& gfx) {
	width = homeWidth;
	height = homeHeight;
	nearZ = homeNearZ;
	farZ = homeFarZ;
	frustum.SetVertices(gfx, width, height, nearZ, farZ);
}
