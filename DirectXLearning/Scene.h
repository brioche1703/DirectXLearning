#pragma once

#include "Model.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

class Graphics;
class MP;

namespace Rgph {
	class RenderGraph;
}

class Scene {
public:
	Scene();

	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel);

	static void AddModel(std::string name, Graphics& gfx, Rgph::RenderGraph& rg, std::string path, const float scale = 1.0f) noexcept;
	std::shared_ptr<Model> GetModel(std::string name) noexcept;
	void SpawnProbeWindow(std::string name) noexcept;
	void SpawnAllProbeWindow() noexcept;

private:
	static std::map<std::string, std::shared_ptr<Model>> models;
	static std::map<std::string, std::shared_ptr<MP>> modelsProbes;
};
