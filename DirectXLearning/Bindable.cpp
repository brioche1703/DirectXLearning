#include "Bindable.h"

#include <stdexcept>

namespace Bind {
	ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
	{
		return gfx.pContext.Get();
	}

	ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
	{
		return gfx.pDevice.Get();
	}

	DxgiInfoManager& Bindable::GetInfoManager(Graphics& gfx)
	{
#ifndef NDEBUG
		return gfx.infoManager;
#else
		throw std::logic_error("You're not in debug and still trying to access gfx.infoManager in release!");
#endif // !NDEBUG

	}
}
