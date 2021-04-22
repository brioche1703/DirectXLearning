#pragma once

#include "ConditionalNoexcept.h"

#include <vector>
#include <memory>
#include <string>

class Graphics;
class Pass;
class PassInput;
class PassOutput;
class RenderQueuePass;

namespace Bind {
	class RenderTarget;
	class DepthStencil;
}

class RenderGraph {
public:
	RenderGraph(Graphics& gfx);
	~RenderGraph();

	RenderQueuePass& GetRenderQueue(const std::string& passName);

	void Execute(Graphics& gfx) noxnd;
	void Reset() noexcept;

protected:
	void SetSinkTarget(const std::string& sinkName, const std::string& target);
	void AddGlobalSource(std::unique_ptr<PassOutput>);
	void AddGlobalSink(std::unique_ptr<PassInput>);
	void AppendPass(std::unique_ptr<Pass> pass);
	void Finalize();

private:
	void LinkPassInputs(Pass& pass);
	void LinkGlobalSinks();

private:
	std::vector<std::unique_ptr<Pass>> passes;
	std::vector<std::unique_ptr<PassInput>> globalSinks;
	std::vector<std::unique_ptr<PassOutput>> globalSources;
	std::shared_ptr<Bind::RenderTarget> backBufferTarget;
	std::shared_ptr<Bind::DepthStencil> masterDepth;
	bool finalized = false;
};
