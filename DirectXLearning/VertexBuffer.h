#pragma once

#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "Bindable.h"
#include "Vertex.h"

#include <memory>

namespace Bind {
	class VertexBuffer : public Bindable {
	public:
		VertexBuffer(Graphics& gfx, const dxLearning::VertexBuffer& vbuf);
		VertexBuffer(Graphics& gfx, const std::string& tag, const dxLearning::VertexBuffer& vbuf);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag, const dxLearning::VertexBuffer& vbuf);

		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) {
			return GenerateUID_(tag);
		}

		std::string GetUID() const noexcept override;

	private:
		static std::string GenerateUID_(const std::string& tag);

	private:
		std::string tag;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
}
