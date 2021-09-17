#pragma once

#include "ECS.h"

class LightSystem : public Ecs::System {
public:
	void Init() {}


	void BindLights(Graphics& gfx, DirectX::FXMMATRIX view) {
		for (auto& entity : entities) {
			auto& light = gCoordinator.GetComponent<std::shared_ptr<PointLight>>(entity);
			light->Bind(gfx, view);
		}
	}
};
