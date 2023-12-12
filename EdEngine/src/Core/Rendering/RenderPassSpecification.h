#pragma once

#include <string>
#include <memory>
#include <glm/ext/matrix_float4x4.hpp>
#include "Types.h"

struct RenderPassSpecification
{
	std::string Name;
	
	std::shared_ptr<class BaseFramebuffer> Framebuffer;

	std::shared_ptr<class Shader> Shader;
	glm::vec3 ViewPosition = glm::vec3(0.0f);

	bool bUseBlending = true;
	bool bUseDepthTesting = true;

	DepthTestFunction DepthFunction = DepthTestFunction::Lesser;
	BlendFactor SourceFactor = BlendFactor::SourceAlpha;
	BlendFactor DestinationFactor = BlendFactor::OneMinusSourceAlpha;

	bool bClearColors = false;
	bool bClearDepth = false;
};