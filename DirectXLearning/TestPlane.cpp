#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "Channels.h"

#include "external/imgui/imgui.h"

TestPlane::TestPlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color) 
	:
	pmc({ color })
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$plane." + std::to_string(size);

	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCBuf>(gfx);

	{
		Technique shade("Shade", Chan::main);
		{
			Step only("lambertian");

			only.AddBindable(Texture::Resolve(gfx, "src\\images\\ground.jpg"));
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "ShadowTest_VS.cso");
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "ShadowTest_PS.cso"));

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float3>("specularColor");
			lay.Add<Dcb::Float>("specularWeight");
			lay.Add<Dcb::Float>("specularGloss");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["specularColor"] = dx::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			only.AddBindable(Rasterizer::Resolve(gfx, false));
			only.AddBindable(tcb);

			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}

	{
		Technique map("ShadowMap", Chan::shadow, true);
		{
			Step draw("shadowMap");
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "Solid_VS.cso")));
			draw.AddBindable(std::make_shared<TransformCBuf>(gfx));
			map.AddStep(std::move(draw));
		}
		AddTechnique(std::move(map));
	}
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept {
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestPlane::SpawnControlWindow(Graphics& gfx, const std::string& title) noexcept {
	if (ImGui::Begin(title.c_str())) {
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		//ImGui::Text("Shading");
		//auto pBlender = QueryBindable<Bind::Blender>();
		//float factor = pBlender->GetFactor();
		//ImGui::SliderFloat("Translucensy", &factor, 0.0f, 1.0f);
		//pBlender->SetFactor(factor);
	}
	ImGui::End();
}
