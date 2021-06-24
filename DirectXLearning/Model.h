#pragma once

#include "Graphics.h"
#include "SceneEntity.h"

#include <string>
#include <filesystem>
#include <vector>

class Mesh;
class Node;
class MP;

struct aiMesh;
struct aiMaterial;
struct aiNode;

namespace Rgph {
	class RenderGraph;
}


class Model : public SceneEntity {
public:
	Model(Graphics& gfx, const std::string name, const std::string& pathString, const float scale = 1.0f);
	Model& operator=(const Model& other) {
		name = other.name;
	}
	~Model() noexcept;

	void Submit(size_t channels) const noxnd override;
	void LinkTechniques(Rgph::RenderGraph&) override;
	const std::string& GetName() const noexcept override;
	void OnImguiRender(bool enable) noexcept override;

	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	const DirectX::XMFLOAT4X4 GetRootTransform() const noexcept;
	void Accept(class ModelProbe& probe);

private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;

private:
	std::string name;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;

	std::unique_ptr<MP> probe;
};
