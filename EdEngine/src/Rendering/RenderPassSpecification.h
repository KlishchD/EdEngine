#pragma once

#include <string>
#include <memory>
#include <glm/ext/matrix_float4x4.hpp>
#include <Utils/RenderAPIUtils.h>

struct RenderPassSpecification
{
	std::string name;
	std::shared_ptr<class BaseFramebuffer> framebuffer;
	std::shared_ptr<class Shader> shader;
	glm::mat4 projectionViewMatrix = glm::mat4(1.0f);
	glm::vec3 viewPosition = glm::vec3(0.0f);

	bool blending = true;
	bool depthTest = true;

	BlendFactor sourceFactor = BlendFactor::SourceAlpha;
	BlendFactor destinationFactor = BlendFactor::OneMinusSourceAlpha;

	bool clearColors = false;
	bool clearDepth = false;
};