#pragma once

class Graphics;

namespace Bind {
	class BufferResource {
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer(Graphics& gfx) noexcept = 0;
		virtual void BindAsBuffer(Graphics& gfx, BufferResource*) noexcept = 0;
		virtual void Clear(Graphics& gfx) noexcept = 0;

	};
}
