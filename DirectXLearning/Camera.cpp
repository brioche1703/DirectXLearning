#include "Camera.h"
#include "MathsUtils.h"
#include "external/imgui/imgui.h"

#include <algorithm>

namespace dx = DirectX;
 
Camera::Camera() noexcept {
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept {
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	const auto camPosition = XMLoadFloat3(&pos);
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWindow() noexcept {
	if (ImGui::Begin("Camera")) {
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset")) {
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept {
	pos = { -10.0f, 0.0f, -20.0f };
	pitch = 0.0f;
	yaw = 0.45f;
}

void Camera::Rotate(float dx, float dy) noexcept {
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept {
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));

	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z,
	};
}
