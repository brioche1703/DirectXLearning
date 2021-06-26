#pragma once

#include "ECS.h"
#include "Model.h"

#include <DirectXMath.h>

extern Ecs::Coordinator gCoordinator;

class SceneSystem : public Ecs::System {
public:
	void Init();

	void Update(float dt);
};

