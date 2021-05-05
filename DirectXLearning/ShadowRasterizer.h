#pragma once

#include "Bindable.h"

namespace Bind {
	class ShadowRasterizer : public Bindable {
	public:
		ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		void ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		void Bind(Graphics& gfx) noxnd override;
		int GetDepthBias() const;
		float GetSlopBias() const;
		float GetClamp() const;

	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		int depthBias;
		float slopeBias;
		float clamp;
	};
}

