#pragma once

#include "Graphics.h"

#include <string>
#include <filesystem>
#include <vector>

class FrameCommander;
class Mesh;
class Node;

struct aiMesh;
struct aiMaterial;
struct aiNode;


class Model {
public:
	Model(Graphics& gfx, const std::string& pathString, const float scale = 1.0f);
	~Model() noexcept;

	void Submit(FrameCommander& frame) const noxnd;
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void Accept(class ModelProbe& probe);

private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;

private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};
