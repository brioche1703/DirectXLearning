#include "TransformCBufScaling.h"

#include "TechniqueProbe.h"

namespace dx = DirectX;

Bind::TransformCBufScaling::TransformCBufScaling(Graphics& gfx, float scale) 
	:
	TransformCBuf(gfx),
	buf(MakeLayout())
{
	buf["scale"] = scale;
}

void Bind::TransformCBufScaling::Accept(TechniqueProbe& probe) {
	probe.VisitBuffer(buf);
}

void Bind::TransformCBufScaling::Bind(Graphics& gfx) noexcept {

	const float scale = buf["scale"];
	const auto scaleMatrix = dx::XMMatrixScaling(scale, scale, scale);
	auto xf = GetTransforms(gfx);
	xf.modelView = xf.modelView * scaleMatrix;
	xf.modelViewProj = xf.modelViewProj * scaleMatrix;
	UpdateBindImpl(gfx, xf);
}

std::unique_ptr<Bind::CloningBindable> Bind::TransformCBufScaling::Clone() const noexcept
{
	return std::make_unique<TransformCBufScaling>(*this);
}

Dcb::RawLayout Bind::TransformCBufScaling::MakeLayout() {
	Dcb::RawLayout lay;
	lay.Add<Dcb::Float>("scale");
	return lay;
}
