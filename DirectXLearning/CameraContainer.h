#pragma once

#include "ECS.h"

#include <vector>
#include <memory>


class Camera;
class Graphics;
class CameraSystem;

namespace Rgph {
	class RenderGraph;
}

class CameraContainer {
public:
	CameraContainer();
	~CameraContainer();

	Camera* operator->();

	void AddCamera(std::shared_ptr<Camera> pCam);
	Camera& GetActiveCamera();

	void Bind(Graphics& gfx);
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channels) const;

	void SpawnWindow(Graphics& gfx);

	void TravelingTest(float dt);

	std::shared_ptr<CameraSystem> cameraSystem;
private:
	Camera& GetControlledCamera();

private:
	std::shared_ptr<Camera> activeCamera;
	std::shared_ptr<Camera> controlledCamera;
};
