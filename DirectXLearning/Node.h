#pragma once

#include "Graphics.h"

#include <string>
#include <vector>
#include <memory>

class Mesh;
class ModelProbe;
class TechniqueProbe;

class Node {
	friend class Model;

public:
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4 GetAppliedTransform() const noexcept;
	int GetId() const noexcept;

	bool HasChildren() const noexcept {
		return childPtrs.size() > 0;
	}

	void Accept(ModelProbe& probe);
	void Accept(TechniqueProbe& probe);

	const std::string& GetName() const {
		return name;
	}

private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	int id;
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};
