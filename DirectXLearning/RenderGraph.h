#pragma once

#include "ConditionalNoexcept.h"

#include <vector>
#include <memory>
#include <string>

class Graphics;

namespace Bind {
	class RenderTarget;
	class DepthStencil;
}


namespace Rgph {
	class Pass;
	class Source;
	class Sink;
	class RenderQueuePass;

	class RenderGraph {
	public:
		RenderGraph(Graphics& gfx);
		~RenderGraph();

		RenderQueuePass& GetRenderQueue(const std::string& passName);

		void Execute(Graphics& gfx) noxnd;
		void Reset() noexcept;

	protected:
		void SetSinkTarget(const std::string& sinkName, const std::string& target);
		void AddGlobalSource(std::unique_ptr<Source>);
		void AddGlobalSink(std::unique_ptr<Sink>);
		void AppendPass(std::unique_ptr<Pass> pass);
		void Finalize();
		Pass& FindPassByName(const std::string& name);
		std::shared_ptr<Bind::RenderTarget> backBufferTarget;
		std::shared_ptr<Bind::DepthStencil> masterDepth;

	private:
		void LinkSinks(Pass& pass);
		void LinkGlobalSinks();

	private:
		std::vector<std::unique_ptr<Pass>> passes;
		std::vector<std::unique_ptr<Sink>> globalSinks;
		std::vector<std::unique_ptr<Source>> globalSources;
		bool finalized = false;
	};
}
