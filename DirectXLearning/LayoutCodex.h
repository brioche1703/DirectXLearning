#pragma once

#include "DynamicConstant.h"

#include <unordered_map>
#include <memory>

namespace Dcb {
	class LayoutCodex {
	public:
		static CookedLayout Resolve(RawLayout&& layout) noxnd;
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};
}
