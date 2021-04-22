#pragma once

#include "RenderQueuePass.h"

class LambertianPass : public RenderQueuePass {
public:
	LambertianPass(std::string name) 
		:
		RenderQueuePass(std::move(name))
	{
		RegisterInput(BufferInput<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterInput(BufferInput<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		RegisterOutput(BufferOutput<Bind::RenderTarget>::Make("renderTarget", renderTarget));
	}

private:
	std::vector<Job> jobs;
};
