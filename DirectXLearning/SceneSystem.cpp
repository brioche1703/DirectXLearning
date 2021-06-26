#include "SceneSystem.h"

void SceneSystem::Init() {}

void SceneSystem::Update(float dt) {
	for (auto const& entity : entities) {
		auto& model = gCoordinator.GetComponent<std::shared_ptr<Model>>(entity);
		DirectX::XMFLOAT4X4 f = model->GetRootTransform();
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&f);
		if (model->GetName() != "sponza") {
			model->SetRootTransform(
				DirectX::XMMatrixScaling(1.001f, 1.001f, 1.001f) *
				m
			);
		}
	}
}
