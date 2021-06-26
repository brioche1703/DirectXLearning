#pragma once

#include "Projection.h"
#include "CameraIndicator.h"
#include "SceneEntity.h"

#include <DirectXMath.h>
#include <string>

class Graphics;

class Camera : public SceneEntity{
public:
	Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos = { 0.0f, 0.0f, 0.0f }, float homePitch = 0.0f, float homeYaw = 0.0f, bool tethered = false) noexcept;
	void BindToGraphics(Graphics& gfx) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset(Graphics& gfx) noexcept;
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;

	const std::string& GetName() const noexcept override;
	void LinkTechniques(Rgph::RenderGraph& rg) override;
	void Submit(size_t channel) const noxnd override;
	void OnImguiRender(bool enable) noexcept override;

private:
	Graphics& gfx;
	bool tethered;
	DirectX::XMFLOAT3 homePos;
	float homePitch;
	float homeYaw;
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	bool enableCameraIndicator = true;
	bool enableFrustumIndicator = true;
	Projection proj;
	CameraIndicator indicator;
};
