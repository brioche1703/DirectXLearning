#pragma once

#include "Drawable.h"
#include "BindableCommon.h"
#include "DirectXException.h"
#include "DynamicConstant.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "external/imgui/imgui.h"

#include <memory>
#include <optional>
#include <type_traits>
#include <filesystem>

class ModelException : public DirectXException {
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;

private:
	std::string note;
};

class Mesh : public Drawable {
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindsPtr);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node {
	friend class Model;

public:
	public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4 GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
	const Dcb::Buffer* GetMaterialConstants() const noxnd;
	void SetMaterialConstants(const Dcb::Buffer& buf_in) noxnd;

	private:
		void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);

	private:
		int id;
		std::string name;
		std::vector<std::unique_ptr<Node>> childPtrs;
		std::vector<Mesh*> meshPtrs;
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMFLOAT4X4 appliedTransform;
	};

	class Model {
	public:
		Model(Graphics& gfx, const std::string& pathString, const float scale = 1.0f);
		~Model() noexcept;

		void Draw(Graphics& gfx) const noxnd;
		void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;

	private:
		static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
		std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

	private:
		std::unique_ptr<Node> pRoot;
		std::vector<std::unique_ptr<Mesh>> meshPtrs;
		std::unique_ptr<class ModelWindow> pWindow;
	};
