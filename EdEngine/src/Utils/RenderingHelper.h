#pragma once

#include <memory>
#include "Core/Window.h"
#include "Core/Rendering/Types.h"

#undef CreateWindow

class VertexBuffer;
class IndexBuffer;
class Framebuffer;
class CubeFramebuffer;
class RenderingContext;

struct Texture2DDescriptor;
struct Texture2DImportParameters;
struct Texture2DData;
class Texture2D;

struct CubeTextureDescriptor;
struct CubeTextureImportParameters;
struct CubeTextureData;
class CubeTexture;

class Shader;

class RenderingHelper
{
public:
	static std::shared_ptr<Window> CreateWindow(WindowSpecification specificeton);

	static std::shared_ptr<VertexBuffer> CreateVertexBuffer(void* data, uint32_t size, const class VertexBufferLayout& layout, BufferUsage usage);
	static std::shared_ptr<VertexBuffer> CreateCubeVertexBuffer();

	static std::shared_ptr<IndexBuffer> CreateIndexBuffer(void* data, uint32_t size, BufferUsage usage);

	static std::shared_ptr<Shader> CreateShader(const std::string& path);

	static std::shared_ptr<Framebuffer> CreateFramebuffer(uint32_t width, uint32_t height);
	static std::shared_ptr<CubeFramebuffer> CreateCubeFramebuffer(uint32_t size);

	static std::shared_ptr<Texture2D> CreateTexture2D(const std::shared_ptr<Texture2DDescriptor>& descriptor);
	static std::shared_ptr<Texture2D> CreateTexture2D(Texture2DImportParameters& parameters, Texture2DData& data, const std::string& name);
	static std::shared_ptr<Texture2D> CreateTexture2D(Texture2DImportParameters&& parameters, Texture2DData&& data, const std::string& name);

	static std::shared_ptr<CubeTexture> CreateCubeTexture(const std::shared_ptr<CubeTextureDescriptor>& descriptor);
	static std::shared_ptr<CubeTexture> CreateCubeTexture(CubeTextureImportParameters& parameters, CubeTextureData& data, const std::string& name);
	static std::shared_ptr<CubeTexture> CreateCubeTexture(CubeTextureImportParameters&& parameters, CubeTextureData&& data, const std::string& name);

	static std::shared_ptr<Texture2D> CreateBloomIntermediateTexture();

	static inline std::shared_ptr<Texture2D> WhiteTexture;

	static std::shared_ptr<Texture2D> GetWhiteTexture();
	static std::shared_ptr<Texture2D> GetBlackTexture();

	static Texture2DImportParameters GetDefaultBaseColorTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultNormalTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultMetalicTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultRoughnessTexture2DImportParameters(const std::string& path);
};