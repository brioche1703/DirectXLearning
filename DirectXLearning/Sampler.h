#pragma once

#include "Bindable.h"

#include <memory>

namespace Bind {
	class Sampler : public Bindable {
	public:
		Sampler(Graphics& gfx);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<Sampler> Resolve(Graphics& gfx);
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}
