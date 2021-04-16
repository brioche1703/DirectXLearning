#pragma once

#include "Graphics.h"
#include "BindableCommon.h"
#include "Technique.h"

#include <assimp/scene.h>
#include <filesystem>
#include <vector>

class Material {
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;
	dxLearning::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noxnd;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	std::vector<Technique> GetTechniques() const noexcept;

private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;

private:
	dxLearning::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};
