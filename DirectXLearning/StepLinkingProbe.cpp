#include "StepLinkingProbe.h"

#include "Step.h"

void StepLinkingProbe::OnSetStep() {
	pStep->Link(rg);
}
