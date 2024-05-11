#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

class SSAOMultiPass : public MultiPassRenderPass<MultiRenderPassParameters, ShaderParameters>
{
public:
	virtual void Execute() override;

protected:
	virtual void CreatePasses();
};