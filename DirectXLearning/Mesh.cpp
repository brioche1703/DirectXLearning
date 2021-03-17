#include "Mesh.h"
#include "Surface.h"
#include "Sampler.h"
#include "ConditionalNoexcept.h"
#include "external/imgui/imgui.h"

#include <sstream>
#include <unordered_map>

	ModelException::ModelException(int line, const char* file, std::string note) noexcept 
		:
		DirectXException(line, file),
		note(std::move(note))
	{}

	const char* ModelException::what() const noexcept {
		std::ostringstream oss;
		oss << DirectXException::what() << std::endl
			<< "[Note] " << GetNote();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* ModelException::GetType() const noexcept {
		return "DirectX Model Exception";
	}

	const std::string& ModelException::GetNote() const noexcept {
		return note;
	}

	Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindsPtr) {
		AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		for (auto& pb : bindsPtr) {
			AddBind(std::move(pb));
		}
		AddBind(std::make_shared<Bind::TransformCBuf>(gfx, *this));
	}

	void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd {
		DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
		Drawable::Draw(gfx);
	}

	DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
		return DirectX::XMLoadFloat4x4(&transform);
	}

	Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
		:
		id(id),
		name(name),
		meshPtrs(std::move(meshPtrs))
	{
		DirectX::XMStoreFloat4x4(&transform, transform_in);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}

	void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd {
		const auto built = 
			DirectX::XMLoadFloat4x4(&appliedTransform) *
			DirectX::XMLoadFloat4x4(&transform) *
			accumulatedTransform;

		for (const auto pm : meshPtrs) {
			pm->Draw(gfx, built);
		}
		for (const auto& pc : childPtrs) {
			pc->Draw(gfx, built);
		}
	}

	void Node::AddChild(std::unique_ptr<Node> pChild) noxnd {
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}

	void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept {
		DirectX::XMStoreFloat4x4(&appliedTransform, transform);
	}

	int Node::GetId() const noexcept {
		return id;
	}

	void Node::ShowTree(Node*& pSelectedNode) const noexcept {
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
			| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

		const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str());
		if (ImGui::IsItemClicked()) {
			pSelectedNode = const_cast<Node*>(this);
		}
		if (expanded) {
			for (const auto& pChild : childPtrs) {
				pChild->ShowTree(pSelectedNode);
			}
			ImGui::TreePop();
		}
	}

	class ModelWindow {
	public:
		void Show(const char* windowName, const Node& root) noexcept {
			windowName = windowName ? windowName : "Model";

			int nodeIndexTracked = 0;
			if (ImGui::Begin(windowName))
			{
				ImGui::Columns(2, nullptr, true);
				root.ShowTree(pSelectedNode);

				ImGui::NextColumn();
				if (pSelectedNode != nullptr) {
					auto& transform = transforms[pSelectedNode->GetId()];
					ImGui::Text("Orientation");
					ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
					ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
					ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
					ImGui::Text("Position");
					ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
					ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
					ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
				}
			}
			ImGui::End();
		}

		DirectX::XMMATRIX GetTransform() const noexcept {
			assert(pSelectedNode != nullptr);
			const auto& transform = transforms.at(pSelectedNode->GetId());
			return 
				DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) * 
				DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
		}

		Node* GetSelectedNode() const noexcept {
			return pSelectedNode;
		}

	private:
		std::optional<int> selectedIndex;
		Node* pSelectedNode;

		struct TransformParameters
		{
			float roll = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters> transforms;
	};

	Model::Model(Graphics& gfx, const std::string filename)
		:
		pWindow(std::make_unique<ModelWindow>())
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(
			filename.c_str(),
			aiProcess_Triangulate
			| aiProcess_ConvertToLeftHanded
			| aiProcess_GenNormals);

		if (pScene == nullptr) {
			throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++) {
			meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
		}

		int nextId = 0;
		pRoot = ParseNode(nextId, *pScene->mRootNode);
	}

	Model::~Model() noexcept {}

	std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials) {
		namespace dx = DirectX;
		using dxLearning::VertexLayout;
		using namespace Bind;

		dxLearning::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		std::vector<std::shared_ptr<Bindable>> bindablePtrs;
		using namespace std::string_literals;
		const auto path = "src\\models\\nano_textured\\"s;

		bool hasSpecularMap = false;
		float shininess = 35.0f;
		if (mesh.mMaterialIndex >= 0) {
			auto& material = *pMaterials[mesh.mMaterialIndex];
			aiString texFilename;
			material.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename);
			bindablePtrs.push_back(Texture::Resolve(gfx, path + texFilename.C_Str()));

			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS) {
				bindablePtrs.push_back(Texture::Resolve(gfx, path + texFilename.C_Str(), 1));
				hasSpecularMap = true;
			}
			else {
				material.Get(AI_MATKEY_SHININESS, shininess);
			}
			bindablePtrs.push_back(Sampler::Resolve(gfx));
		}

		auto meshTag = path + "%" + mesh.mName.C_Str();

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		if (hasSpecularMap) {
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongSpecMapPS.cso"));
		}
		else {
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		}
		
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstant {
			float specularIntensity = 1.6f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}

	std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept {
		namespace dx = DirectX;
		const auto transform = dx::XMMatrixTranspose(
			dx::XMLoadFloat4x4(
				reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
			));

		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);
		for (size_t i = 0; i < node.mNumMeshes; i++) {
			const auto meshIdx = node.mMeshes[i];
			curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
		}

		auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
		for (size_t i = 0; i < node.mNumChildren; i++) {
			pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
		}

		return pNode;
	}

	void Model::Draw(Graphics& gfx) const noxnd {
		if (auto node = pWindow->GetSelectedNode()) {
			node->SetAppliedTransform(pWindow->GetTransform());
		}
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}

	void Model::ShowWindow(const char* windowName) noexcept {
		pWindow->Show(windowName, *pRoot);
	}
