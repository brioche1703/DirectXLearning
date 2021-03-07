#include "ModelTest.h"
#include "BindableCommon.h"
#include "Vertex.h"

#include "external/imgui/imgui.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

ModelTest::ModelTest(
	Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized()) {
		using dxLearning::VertexLayout;
		dxLearning::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));


		Assimp::Importer imp;
		const auto pModel = imp.ReadFile(
			"src\\models\\suzanne.obj",
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

		const auto pMesh = pModel->mMeshes[0];

		for (unsigned int i = 0; i < pMesh->mNumVertices; i++) {
			vbuf.EmplaceBack(
				dx::XMFLOAT3{
					pMesh->mVertices[i].x * scale,
					pMesh->mVertices[i].y * scale,
					pMesh->mVertices[i].z * scale,
				},
				*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i]));
		}

		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

		AddStaticIndexBuffer(std::make_unique<Bind::IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticBind(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCBuf>(gfx, materialConstants, 1u));

	AddBind(std::make_unique<Bind::TransformCBuf>(gfx, *this));
}

bool ModelTest::SpawnControlWindow(int id, Graphics& gfx) noexcept {
	using namespace std::string_literals;

	bool dirty = false;
	bool open = true;
	if (ImGui::Begin(("Model"s + std::to_string(id)).c_str(), &open)) {
		ImGui::Text("Material Properties");
		const auto cd = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		const auto sid = ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f", 2);
		const auto spd = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", 2);
		dirty = cd || sid || spd;
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();
	
	if (dirty) {
		SyncMaterial(gfx);
	}
	return open;
}

void ModelTest::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG) {
	auto pConstPS = QueryBindable<MaterialCBuf>();
	assert(pConstPS != nullptr);
	pConstPS->Update(gfx, materialConstants);
}
