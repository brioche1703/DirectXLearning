#include "LayoutCodex.h"

namespace Dcb {
	CookedLayout LayoutCodex::Resolve(RawLayout&& layout) noxnd {
		auto sig = layout.GetSignature();
		auto& map = Get_().map;
		const auto i = map.find(sig);

		if (i != map.end()) {
			layout.ClearRoot();
			return { i->second };
		}

		auto result = map.insert({ std::move(sig), layout.DeliverRoot() });
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get_() noexcept {
		static LayoutCodex codex;
		return codex;
	}
}