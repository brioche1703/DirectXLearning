#pragma once

#pragma once
#include "TestObject.h"
#include "ConstantBuffers.h"

class ModelTest : public TestObject<ModelTest>
{
public:
	ModelTest(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		DirectX::XMFLOAT3 material,
		float scale);


	bool SpawnControlWindow(int id, Graphics& gfx) noexcept;

private:
	void SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG);

private:
	struct PSMaterialConstant{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} materialConstants;

	using MaterialCBuf = Bind::PixelConstantBuffer<PSMaterialConstant>;
};
