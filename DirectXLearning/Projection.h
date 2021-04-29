#pragma once

#include "Frustum.h"

#include <DirectXMath.h>

class Graphics;

namespace Rgph {
	class RenderGraph;
}

class Projection {
public:
	Projection(Graphics& gfx, float width, float height, float nearZ, float farZ);
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets(Graphics& gfx);
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetRotation(DirectX::XMFLOAT3 rot);
	void Submit(size_t channel) const;
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Reset(Graphics& gfx);

private:
	float width;
	float height;
	float nearZ;
	float farZ;
	float homeWidth;
	float homeHeight;
	float homeNearZ;
	float homeFarZ;
	Frustum frustum;
};
