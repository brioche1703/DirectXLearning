#include "Mesh.h"

#include "ConditionalNoexcept.h"

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noxnd 
	:
	Drawable(gfx, mat, mesh, scale)
{}

void Mesh::Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Submit();
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat4x4(&transform);
}
