#pragma once

#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include "Technique.h"

#include <DirectXMath.h>
#include <memory>
#include <vector>

class TechniqueProbe;
class RenderGraph;
class Material;
struct aiMesh;

namespace Bind {
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

namespace Rgph {
	class RenderGraph;
}

class Drawable {

public:
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual ~Drawable();

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Submit() const noexcept;
	void AddTechnique(Technique tech_in) noexcept;
	void Bind(Graphics& gfx) const noxnd;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	void LinkTechniques(Rgph::RenderGraph&);


protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};
