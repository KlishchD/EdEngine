#pragma once

#include <memory>
#include <string>
#include <glm/fwd.hpp>
#include "Core/Rendering/Types.h"

class Texture;
class Shader;
class Framebuffer;
class UniformBuffer;
class VertexBuffer;
class IndexBuffer;

class RenderingContext 
{
public:
	static RenderingContext& Get();

	virtual void SetDefaultFramebuffer() = 0;
	virtual void SetFramebuffer(std::shared_ptr<Framebuffer> framebuffer) = 0;

	virtual void SetUniformBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t location) = 0;
	virtual void SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer) = 0;
	virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer) = 0;

	virtual void SetShader(std::shared_ptr<Shader> shader) = 0;

	virtual void SetShaderDataTexture(const std::string& name, std::shared_ptr<Texture> texture) = 0;
	virtual void SetShaderDataImage(const std::string& name, std::shared_ptr<Texture> texture) = 0;
	virtual void SetShaderDataInt(const std::string& name, int32_t value) = 0;
	virtual void SetShaderDataFloat(const std::string& name, float value) = 0;
	virtual void SetShaderDataFloat2(const std::string& name, glm::vec2 vector) = 0;
	virtual void SetShaderDataFloat2(const std::string& name, float x, float y) = 0;
	virtual void SetShaderDataFloat3(const std::string& name, float x, float y, float z) = 0;
	virtual void SetShaderDataFloat3(const std::string& name, glm::vec3 vector) = 0;
	virtual void SetShaderDataFloat4(const std::string& name, float r, float g, float b, float a) = 0;
	virtual void SetShaderDataFloat4(const std::string& name, glm::vec4 vector) = 0;
	virtual void SetShaderDataMat4(const std::string& name, const glm::mat4& matrix) = 0;
	virtual void SetShaderDataMat3(const std::string& name, const glm::mat3& matrix) = 0;
	virtual void SetShaderDataBool(const std::string& name, bool value) = 0;

	virtual void SetShaderDataTexture(const char* name, std::shared_ptr<Texture> texture) = 0;
	virtual void SetShaderDataImage(const char* name, std::shared_ptr<Texture> texture) = 0;
	virtual void SetShaderDataInt(const char* name, int32_t value) = 0;
	virtual void SetShaderDataFloat(const char* name, float value) = 0;
	virtual void SetShaderDataFloat2(const char* name, glm::vec2 vector) = 0;
	virtual void SetShaderDataFloat2(const char* name, float x, float y) = 0;
	virtual void SetShaderDataFloat3(const char* name, float x, float y, float z) = 0;
	virtual void SetShaderDataFloat3(const char* name, glm::vec3 vector) = 0;
	virtual void SetShaderDataFloat4(const char* name, float r, float g, float b, float a) = 0;
	virtual void SetShaderDataFloat4(const char* name, glm::vec4 vector) = 0;
	virtual void SetShaderDataMat4(const char* name, const glm::mat4& matrix)  = 0;
	virtual void SetShaderDataMat3(const char* name, const glm::mat3& matrix)  = 0;
	virtual void SetShaderDataBool(const char* name, bool value) = 0;

	virtual void RunComputeShader(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) = 0;
	virtual void Barier(BarrierType type) = 0;

	virtual void Draw(DrawMode mode = DrawMode::Triangles) = 0;

	virtual void EnableBlending(BlendFactor source, BlendFactor destination) = 0;
	virtual void SetBlending(BlendFactor source, BlendFactor destination) = 0;
	virtual void DisableBlending() = 0;

	virtual void EnableDethTest(DepthTestFunction function) = 0;
	virtual void SetDethTestFunction(DepthTestFunction function) = 0;
	virtual void DisableDethTest() = 0;

	virtual void EnableFaceCulling() = 0;
	virtual void EnableFaceCulling(Face face) = 0;
	virtual void SetCullingFace(Face face) = 0;
	virtual void DisableFaceCulling() = 0;

	virtual void ClearDepthTarget() = 0;
	virtual void ClearColorTarget() = 0;
	virtual void SetClearColor(float r, float g, float b, float a) = 0;
	virtual void SetClearColor(glm::vec4 color) = 0;

	virtual void BeginUIFrame() = 0;
	virtual void EndUIFrame() = 0;

	virtual void SwapBuffers() = 0;

	virtual ~RenderingContext() = default;
protected:
	static void SetContext(RenderingContext* context);
};