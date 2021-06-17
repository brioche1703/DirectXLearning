#pragma once

#include "ECS.h"

#include <vector>
#include <memory>


class Camera;
class Graphics;

namespace Rgph {
	class RenderGraph;
}

class CameraSystem : public Ecs::System {
	void Init() {}
};

class CameraContainer {
public:
	CameraContainer();
	void SpawnWindow(Graphics& gfx);
	void Bind(Graphics& gfx);
	void AddCamera(std::shared_ptr<Camera> pCam);
	Camera* operator->();
	~CameraContainer();
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channels) const;
	Camera& GetActiveCamera();
	std::shared_ptr<Camera> GetActiveCameraPointer();
	void TravelingTest(float dt);

private:
	Camera& GetControlledCamera();

private:
	std::shared_ptr<CameraSystem> cameraSystem;
	std::shared_ptr<Camera> activeCamera;
	std::shared_ptr<Camera> controlledCamera;
};
