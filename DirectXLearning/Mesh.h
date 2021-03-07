#pragma once

#include "DrawableBase.h"
#include "BindableCommon.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <memory>

class Mesh : public DrawableBase<Mesh> {
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindsPtr);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node {
	friend class Model;

public:
	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);

private:
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model {
public:
	Model(Graphics& gfx, const std::string filename);
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const;

private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};
