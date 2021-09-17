#pragma once

#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "SceneEntity.h"

namespace Rgph {
	class RenderGraph;
}

class Camera;

struct PointLightCBuf {
	alignas(16) DirectX::XMFLOAT3 pos;
	alignas(16) DirectX::XMFLOAT3 ambient;
	alignas(16) DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

class PointLight : public SceneEntity {
public:
	PointLight(Graphics& gfx, std::string name, DirectX::XMFLOAT3 pos = { 10.0f, 9.0f, 2.5f }, DirectX::XMFLOAT3 color = { 0.5f, 0.5f, 0.5f }, float radius = 0.5f);
	void Reset() noexcept;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void SpawnControlWindow(std::string title) noexcept;

	void Submit(size_t channels) const noxnd override;
	void LinkTechniques(Rgph::RenderGraph&) override;
	const std::string& GetName() const noexcept override;
	void OnImguiRender(bool enable) noexcept override;
	PointLightCBuf GetCBuf() const noexcept;

private:
	PointLightCBuf home;
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
};
