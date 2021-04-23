#pragma once

class Graphics;

namespace Bind {
	class BufferResource {
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer(Graphics& gfx) noxnd = 0;
		virtual void BindAsBuffer(Graphics& gfx, BufferResource*) noxnd = 0;
		virtual void Clear(Graphics& gfx) noxnd = 0;

	};
}
