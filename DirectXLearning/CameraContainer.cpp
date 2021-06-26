#include "CameraContainer.h"
#include "CameraSystem.h"
#include "Camera.h"
#include "Graphics.h"
#include "RenderGraph.h"

#include "external/imgui/imgui.h"

extern Ecs::Coordinator gCoordinator;

CameraContainer::CameraContainer() {
	gCoordinator.RegisterComponent<std::shared_ptr<Camera>>();
	
	cameraSystem = gCoordinator.RegisterSystem<CameraSystem>();
	{
		Ecs::Signature signature;
		signature.set(gCoordinator.GetComponentType<std::shared_ptr<Camera>>());
		gCoordinator.SetSystemSignature<CameraSystem>(signature);
	}
}

void CameraContainer::SpawnWindow(Graphics& gfx) {
	if (ImGui::Begin("Cameras")) {
		ImGui::Text(("GPU : " + gfx.GetGpuName()).c_str());
		if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str())) {
			for (auto& entity : cameraSystem->entities) {
				auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
				const bool isSelected = activeCamera == cam;
				if (ImGui::Selectable(cam->GetName().c_str(), isSelected)) {
					activeCamera = cam;
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Controlled Camera", GetControlledCamera().GetName().c_str())) {
			for (auto& entity : cameraSystem->entities) {
				auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
				const bool isSelected = controlledCamera == cam;
				if (ImGui::Selectable(cam->GetName().c_str(), isSelected)) {
					controlledCamera = cam;
				}
			}
			ImGui::EndCombo();
		}
		GetControlledCamera().SpawnControlWidgets(gfx);
	}
	ImGui::End();
}

void CameraContainer::Bind(Graphics& gfx) {
	gfx.SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera(std::shared_ptr<Camera> pCam) {
	if (cameraSystem->entities.size() == 0) {
		activeCamera = pCam;
		controlledCamera = pCam;
	}
	auto entity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(entity, pCam);
}

Camera* CameraContainer::operator->() {
	return &GetActiveCamera();
}

CameraContainer::~CameraContainer() 
{}

void CameraContainer::LinkTechniques(Rgph::RenderGraph & rg) {
	for (auto& entity : cameraSystem->entities) {
		auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
		cam->LinkTechniques(rg);
	}
}

void CameraContainer::Submit(size_t channels) const {
	for (auto& entity : cameraSystem->entities) {
		auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
		if (cam != activeCamera) {
			cam->Submit(channels);
		}
	}
}

Camera& CameraContainer::GetActiveCamera() {
	for (auto& entity : cameraSystem->entities) {
		auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
		if (cam == activeCamera) {
			return *cam;
		}
	}
}

Camera& CameraContainer::GetControlledCamera() {
	for (auto& entity : cameraSystem->entities) {
		auto& cam = gCoordinator.GetComponent<std::shared_ptr<Camera>>(entity);
		if (cam == controlledCamera) {
			return *cam;
		}
	}
}

void CameraContainer::TravelingTest(float dt) {
	float speed = 120.0f;
	activeCamera->Rotate(speed * dt, 0.0f);
}
