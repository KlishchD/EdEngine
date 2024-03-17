#pragma once

#include "Core/Rendering/Passes/RenderPass.h"

class SSAOMultiPass : public MultiPassRenderPass<MultiRenderPassParameters, ShaderParameters>
{
public:
	virtual void Update(float deltaSeconds) override;

protected:
	virtual void CreatePasses();
};