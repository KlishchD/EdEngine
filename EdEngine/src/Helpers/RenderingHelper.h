#pragma once

#include <vector>
#include <memory>
#include "Core/Window.h"
#include "Core/Rendering/Types.h"
#include "Core/Math/Transform.h"
#include "Core/Math/Camera.h"
#include "Helpers/FilesHelper.h"

#include "Core/Assets/ImportParameters/TextureImportParameters.h"

#undef CreateWindow

class VertexBuffer;
class IndexBuffer;
class UniformBuffer;

class RenderingContext;

class Texture;

class Texture2DData;
class Texture2D;

class CubeTextureImportParameters;
class CubeTextureData;
class CubeTexture;

class Texture2DArrayImportParameters;
class Texture2DArrayData;
class Texture2DArray;

class Shader;

class Framebuffer;
struct FramebufferSpecification;
struct RenderTargetSpecification;

class RenderGraph;

enum class FramebufferAttachmentType;

class RenderingHelper
{
public:
	static std::shared_ptr<Window> CreateWindow(WindowSpecification specificeton);

	static std::shared_ptr<VertexBuffer> CreateVertexBuffer(void* data, uint32_t size, const class VertexBufferLayout& layout, BufferUsage usage);
	static std::shared_ptr<VertexBuffer> CreateCubeVertexBuffer();

	static std::shared_ptr<IndexBuffer> CreateIndexBuffer(void* data, uint32_t size, BufferUsage usage);

	static std::shared_ptr<UniformBuffer> CreateUniformBuffer();
	static std::shared_ptr<UniformBuffer> CreateUniformBuffer(void* data, uint32_t size, BufferUsage usage);

	template<typename T>
	static std::shared_ptr<UniformBuffer> CreateUniformBuffer(T* data, BufferUsage usage);

	static std::shared_ptr<Shader> CreateShader(const std::string& path);

	template<typename T>
	static std::shared_ptr<T> CreateRenderTarget(const RenderTargetSpecification& specification, TextureType textureType)
	{
		return std::static_pointer_cast<T>(CreateRenderTarget(specification, textureType));
	}

	static std::shared_ptr<Texture> CreateRenderTarget(const RenderTargetSpecification& specification, TextureType textureType);

	static std::shared_ptr<Framebuffer> CreateFramebuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t depth, const std::vector<RenderTargetSpecification>& renderTargets, TextureType textureType);
	static std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferSpecification& specification);

	static std::shared_ptr<Texture2D> CreateTexture2D(const std::string& name);
	static std::shared_ptr<Texture2D> CreateTexture2D(const std::string& name, std::shared_ptr<Texture2DImportParameters> parameters, Texture2DData&& data);

	static std::shared_ptr<CubeTexture> CreateCubeTexture(const std::string& name);
	static std::shared_ptr<CubeTexture> CreateCubeTexture(const std::string& name, std::shared_ptr<CubeTextureImportParameters> parameters, CubeTextureData&& data);

	static std::shared_ptr<Texture2DArray> CreateTexture2DArray(const std::string& name);
	static std::shared_ptr<Texture2DArray> CreateTexture2DArray(const std::string& name, std::shared_ptr<Texture2DArrayImportParameters> parameters, Texture2DArrayData&& data);
	
	static std::shared_ptr<Texture2D> CreateBloomIntermediateTexture();

	static std::shared_ptr<Texture2D> GetWhiteTexture();

	static std::shared_ptr<Texture2DImportParameters> GetDefaultBaseColorTexture2DImportParameters(const std::string& path);
	static std::shared_ptr<Texture2DImportParameters> GetDefaultNormalTexture2DImportParameters(const std::string& path);
	static std::shared_ptr<Texture2DImportParameters> GetDefaultMetalicTexture2DImportParameters(const std::string& path);
	static std::shared_ptr<Texture2DImportParameters> GetDefaultRoughnessTexture2DImportParameters(const std::string& path);

	static std::shared_ptr<Texture2D> ImportBaseColorTexture(const std::string& path);
	static std::shared_ptr<Texture2D> ImportNormalTexture(const std::string& path);
	static std::shared_ptr<Texture2D> ImportMetalicTexture(const std::string& path);
	static std::shared_ptr<Texture2D> ImportRoughnessTexture(const std::string& path);

	static bool IsLightMeshVisible(const std::vector<glm::vec3>& vertices, const Transform& transform, const Camera& camera);

private:
	static inline std::shared_ptr<Texture2D> WhiteTexture;

	static std::shared_ptr<Texture2DImportParameters> GetRenderTargetTexture2DImportParameters(FramebufferAttachmentType type);
	static std::shared_ptr<CubeTextureImportParameters> GetRenderTargetCubeTextureImportParameters(FramebufferAttachmentType type);
	static std::shared_ptr<Texture2DArrayImportParameters> GetRenderTargetTexture2DArrayImportParameters(FramebufferAttachmentType type);
};

#include "RenderingHelper.hpp"