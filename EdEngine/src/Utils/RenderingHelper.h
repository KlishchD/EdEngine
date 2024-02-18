#pragma once

#include <vector>
#include <memory>
#include "Core/Window.h"
#include "Core/Rendering/Types.h"

#undef CreateWindow

class VertexBuffer;
class IndexBuffer;
class Framebuffer;
class RenderingContext;

class Texture;

struct Texture2DDescriptor;
struct Texture2DImportParameters;
class Texture2DData;
class Texture2D;

struct CubeTextureDescriptor;
struct CubeTextureImportParameters;
class CubeTextureData;
class CubeTexture;

struct Texture2DArrayDescriptor;
struct Texture2DArrayImportParameters;
class Texture2DArrayData;
class Texture2DArray;

class Shader;

enum class FramebufferAttachmentType
{
	Color,
	Color16,
	Depth,
	DepthStencil,
	Position,
	Direction,
	Velocity,
	Distance,
	Bloom
};

class RenderingHelper
{
public:
	static std::shared_ptr<Window> CreateWindow(WindowSpecification specificeton);

	static std::shared_ptr<VertexBuffer> CreateVertexBuffer(void* data, uint32_t size, const class VertexBufferLayout& layout, BufferUsage usage);
	static std::shared_ptr<VertexBuffer> CreateCubeVertexBuffer();

	static std::shared_ptr<IndexBuffer> CreateIndexBuffer(void* data, uint32_t size, BufferUsage usage);

	static std::shared_ptr<Shader> CreateShader(const std::string& path);

	static std::shared_ptr<Texture> CreateDefaultFramebufferAttachment(FramebufferAttachmentType type, TextureType textureType);

	static std::shared_ptr<Framebuffer> CreateFramebuffer(uint32_t width, uint32_t height, uint32_t depth);
	static std::shared_ptr<Framebuffer> CreateFramebuffer(uint32_t width, uint32_t height, uint32_t depth, const std::vector<FramebufferAttachmentType>& initialAttachments, TextureType textureType);

	static std::shared_ptr<Texture2D> CreateTexture2D(std::shared_ptr<Texture2DDescriptor> descriptor);
	static std::shared_ptr<Texture2D> CreateTexture2D(const Texture2DImportParameters& parameters, const Texture2DData& data, const std::string& name);
	static std::shared_ptr<Texture2D> CreateTexture2D(Texture2DImportParameters&& parameters, Texture2DData&& data, const std::string& name);

	static std::shared_ptr<CubeTexture> CreateCubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor);
	static std::shared_ptr<CubeTexture> CreateCubeTexture(const CubeTextureImportParameters& parameters, const CubeTextureData& data, const std::string& name);
	static std::shared_ptr<CubeTexture> CreateCubeTexture(CubeTextureImportParameters&& parameters, CubeTextureData&& data, const std::string& name);

	static std::shared_ptr<Texture2DArray> CreateTexture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descriptor);
	static std::shared_ptr<Texture2DArray> CreateTexture2DArray(const Texture2DArrayImportParameters& parameters, const Texture2DArrayData& data, const std::string& name);
	static std::shared_ptr<Texture2DArray> CreateTexture2DArray(Texture2DArrayImportParameters&& parameters, Texture2DArrayData&& data, const std::string& name);


	static std::shared_ptr<Texture2D> CreateBloomIntermediateTexture();

	static inline std::shared_ptr<Texture2D> WhiteTexture;

	static std::shared_ptr<Texture2D> GetWhiteTexture();

	static Texture2DImportParameters GetDefaultBaseColorTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultNormalTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultMetalicTexture2DImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultRoughnessTexture2DImportParameters(const std::string& path);
};