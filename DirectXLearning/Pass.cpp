#include "Pass.h"
#include "RenderGraphCompileException.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Utils.h"
#include "Sink.h"
#include "Source.h"

#include <sstream>

namespace Rgph {
	Pass::Pass(std::string name) noexcept
		:
		name(std::move(name))
	{}

	void Pass::Reset() noxnd
	{}

	const std::string& Pass::GetName() const noexcept {
		return name;
	}

	const std::vector<std::unique_ptr<Sink>>& Pass::GetSinks() const {
		return sinks;
	}

	Source& Pass::GetSource(const std::string& registeredName) const {
		for (auto& source : sources) {
			if (source->GetName() == registeredName) {
				return *source;
			}
		}
		std::ostringstream oss;
		oss << "Source name [" << registeredName << "] not found in pass: " << GetName();
		throw RGC_EXCEPTION(oss.str());
	}

	Sink& Pass::GetSink (const std::string& registeredName) const {
		for (auto& sink : sinks) {
			if (sink->GetRegisteredName() == registeredName) {
				return *sink;
			}
		}
		std::ostringstream oss;
		oss << "Sink name [" << registeredName << "] not found in pass: " << GetName();
		throw RGC_EXCEPTION(oss.str());

	}

	void Pass::SetSinkLinkage(const std::string& registeredName, const std::string& target) {
		auto& sink = GetSink(registeredName);
		auto targetSplit = SplitString(target, ".");
		if (targetSplit.size() != 2u) {
			throw RGC_EXCEPTION("Sink target has incorrect format");
		}
		sink.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}

	void Pass::Finalize() {
		for (auto& si : sinks) {
			si->PostLinkValidate();
		}
		for (auto& src : sources) {
			src->PostLinkValidate();
		}
	}

	Pass::~Pass()
	{}

	void Pass::RegisterSink(std::unique_ptr<Sink> sink) {
		for (auto& si : sinks) {
			if (si->GetRegisteredName() == sink->GetRegisteredName()) {
				throw RGC_EXCEPTION("Registered sink overlaps with existing: " + sink->GetRegisteredName());
			}
		}
		sinks.push_back(std::move(sink));
	}

	void Pass::RegisterSource(std::unique_ptr<Source> source) {
		for (auto& src : sources) {
			if (src->GetName() == source->GetName()) {
				throw RGC_EXCEPTION("Registered source overlaps with existing: " + source->GetName());
			}
		}
		sources.push_back(std::move(source));
	}
}
