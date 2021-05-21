#include "PointLight.h"
#include "Camera.h"
#include "MathsUtils.h"
#include "TechniqueProbe.h"
#include "TechniqueProbe.h"
#include "DynamicConstant.h"

#include "external/imgui/imgui.h"

PointLight::PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float radius)
	:
	mesh(gfx, radius, color),
	cbuf(gfx) 
{
	home = {
		pos,
		{0.2f, 0.2f, 0.2f},
		color,
		1.0f,
		1.0f,
		0.025f,
		0.0030f
	};
	Reset();
	pCamera = std::make_shared<Camera>(gfx, "Light", cbData.pos, 0.0f, PI / 2.0f, true);
}

void PointLight::SpawnControlWindow(std::string title) noexcept {
	if (ImGui::Begin(title.c_str())) {
		bool dirtyPos = false;
		const auto dcheck = [&dirtyPos](bool dirty) { dirtyPos = dirtyPos || dirty; };

		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f"));

		if (dirtyPos) {
			pCamera->SetPos(cbData.pos);
		}

		ImGui::Text("Intensity/Color");
		bool dirtyColor = false;
		const auto ccheck = [&dirtyColor](bool dirty) { dirtyColor = dirtyColor || dirty; };
		ccheck(ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", 2));
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		class Probe : public TechniqueProbe {
		public:

			Probe(DirectX::XMFLOAT3 *diffuseColor)
				:
				diffuseColor(diffuseColor) {
			}

			bool OnVisitBuffer(Dcb::Buffer& buf) override {
				namespace dx = DirectX;
				float dirty = false;

				const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
				(const char* label) mutable {
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if (auto v = buf["color"]; v.Exists()) {
					dcheck(ImGui::ColorEdit3(tag("Diffuse"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
					*diffuseColor = static_cast<dx::XMFLOAT3>(v);
				}
				return dirty;
			}

		private:
			DirectX::XMFLOAT3 *diffuseColor;
		} probe(&cbData.diffuseColor);

		mesh.Accept(probe);

		if (ImGui::Button("Reset")) {
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept {
	cbData = home;
}

void PointLight::Submit(size_t channel) const noxnd {
	mesh.SetPos(cbData.pos);
	mesh.Submit(channel);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept {
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}

void PointLight::LinkTechniques(Rgph::RenderGraph& rg) {
	mesh.LinkTechniques(rg);
}

std::shared_ptr<Camera> PointLight::ShareCamera() const noexcept {
	return pCamera;
}
