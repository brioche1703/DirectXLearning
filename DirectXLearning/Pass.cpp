#include "Pass.h"
#include "RenderGraphCompileException.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Utils.h"

#include <sstream>

Pass::Pass(std::string name) noexcept
	:
	name(std::move(name))
{}

void Pass::Reset() noxnd 
{}

const std::string& Pass::GetName() const noexcept {
	return name;
}

const std::vector<std::unique_ptr<PassInput>>& Pass::GetInputs() const {
	return inputs;
}

PassOutput& Pass::GetOutput(const std::string& registeredName) const {
	for (auto& out : outputs) {
		if (out->GetName() == registeredName) {
			return *out;
		}
	}
	std::ostringstream oss;
	oss << "Output name [" << registeredName << "] not found in pass: " << GetName();
	throw RGC_EXCEPTION(oss.str());
}

PassInput& Pass::GetInput(const std::string& registeredName) const {
	for (auto& in : inputs) {
		if (in->GetRegisteredName() == registeredName) {
			return *in;
		}
	}
	std::ostringstream oss;
	oss << "Input name [" << name << "] not found in pass: " << GetName();
	throw RGC_EXCEPTION(oss.str());

}

void Pass::SetInputResource(const std::string& registeredName, const std::string& target) {
	auto& input = GetInput(registeredName);
	auto targetSplit = SplitString(target, ".");
	if (targetSplit.size() != 2u) {
		throw RGC_EXCEPTION("Input target has incorrect format");
	}
	input.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
}

void Pass::Finalize() {
	for (auto& in : inputs) {
		in->PostLinkValidate();
	}
	for (auto& out : outputs) {
		out->PostLinkValidate();
	}
}

Pass::~Pass()
{}

void Pass::RegisterInput(std::unique_ptr<PassInput> input) {
	for (auto& in : inputs) {
		if (in->GetRegisteredName() == input->GetRegisteredName()) {
			throw RGC_EXCEPTION("Registered input overlaps with existing: " + input->GetRegisteredName());
		}
	}
	inputs.push_back(std::move(input));
}

void Pass::RegisterOutput(std::unique_ptr<PassOutput> output) {
	for (auto& out : outputs) {
		if (out->GetName() == output->GetName()) {
			throw RGC_EXCEPTION("Registered output overlaps with existing: " + output->GetName());
		}
	}
	outputs.push_back(std::move(output));
}

void Pass::BindBufferResources(Graphics& gfx) const noxnd {
	if (renderTarget){
		renderTarget->BindAsBuffer(gfx, depthStencil.get());
	}
	else {
		depthStencil->BindAsBuffer(gfx);
	}
}



