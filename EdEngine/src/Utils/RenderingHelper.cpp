#include "RenderingHelper.h"
#include "Platform/Rendering/OpenGL/Framebuffers/OpenGLFramebuffer.h"
#include "Platform/Rendering/OpenGL/Framebuffers/OpenGLCubeFramebuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLVertexBuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLIndexBuffer.h"
#include "Platform/Rendering/OpenGL/OpenGLRenderingContex.h"
#include "Platform/Rendering/OpenGL/OpenGLShader.h"
#include "Platform/Rendering/OpenGL/OpenGLWindow.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLCubeTexture.h"
#include "Core/Rendering/Textures/CubeTexture.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Engine.h"
#include "Core/Macros.h"
#include <fstream>

#undef CreateWindow

std::shared_ptr<Window> RenderingHelper::CreateWindow(WindowSpecification specificeton)
{
	return std::make_shared<OpenGLWindow>(specificeton);
}

std::shared_ptr<VertexBuffer> RenderingHelper::CreateVertexBuffer(void* data, uint32_t size, const VertexBufferLayout& layout, BufferUsage usage)
{
	std::shared_ptr<VertexBuffer> buffer = std::make_shared<OpenGLVertexBuffer>();
	buffer->SetData(data, size, usage);
	buffer->SetLayout(layout);

	return buffer;
}

std::shared_ptr<VertexBuffer> RenderingHelper::CreateCubeVertexBuffer()
{
	float data[4 * 6 * 5] = {
		-0.5f, -0.5f, -0.5f, 0, 1,
		 0.5f, -0.5f, -0.5f, 1, 1,
		 0.5f, -0.5f,  0.5f, 1, 0,
		-0.5f, -0.5f,  0.5f, 0, 0,

		-0.5f,  0.5f, -0.5f, 0, 0,
		 0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f,  0.5f, 1, 1,
		-0.5f,  0.5f,  0.5f, 1, 0,

		 0.5f, -0.5f, -0.5f, 1, 0,
		-0.5f, -0.5f, -0.5f, 0, 0,
		-0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f, -0.5f, 1, 1,

		 0.5f, -0.5f,  0.5f, 0, 0,
		-0.5f, -0.5f,  0.5f, 1, 0,
		-0.5f,  0.5f,  0.5f, 1, 1,
		 0.5f,  0.5f,  0.5f, 0, 1,

		-0.5f, -0.5f, -0.5f, 1, 0,
		-0.5f,  0.5f, -0.5f, 1, 1,
		-0.5f,  0.5f,  0.5f, 0, 1,
		-0.5f, -0.5f,  0.5f, 0, 0,

		 0.5f, -0.5f, -0.5f, 0, 0,
		 0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f,  0.5f, 1, 1,
		 0.5f, -0.5f,  0.5f, 1, 0,
	};

	VertexBufferLayout layout = {
			{ "vertices", ShaderDataType::Float3 },
			{ "texCoords", ShaderDataType::Float2 }
	};

	std::shared_ptr<VertexBuffer> buffer = std::make_shared<OpenGLVertexBuffer>();
	buffer->SetData(data, sizeof(data), BufferUsage::StaticDraw);
	buffer->SetLayout(layout);

	return buffer;
}

std::shared_ptr<IndexBuffer> RenderingHelper::CreateIndexBuffer(void* data, uint32_t size, BufferUsage usage)
{
	std::shared_ptr<IndexBuffer> buffer = std::make_shared<OpenGLIndexBuffer>();
	buffer->SetData(data, size, usage);

	return buffer;
}

std::shared_ptr<Shader> RenderingHelper::CreateShader(const std::string& path)
{
	std::string source;
	ShaderType currentShaderType = ShaderType::None;

	std::fstream file(path, std::ios_base::in);
	std::string line;

	ED_ASSERT(file.is_open(), "Couldn't find a shader")

	std::shared_ptr<OpenGLShader> shader = std::make_shared<OpenGLShader>();

	while (std::getline(file, line)) {
		if (line.find("// type") != std::string::npos) {
			if (currentShaderType != ShaderType::None) {
				shader->SetShaderCode(currentShaderType, source);
			}
			source.clear();

			if (line.find("fragment") != std::string::npos) {
				currentShaderType = ShaderType::Pixel;
			}
			else if (line.find("vertex") != std::string::npos) {
				currentShaderType = ShaderType::Vertex;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				currentShaderType = ShaderType::Geometry;
			}
			else if (line.find("compute") != std::string::npos)
			{
				currentShaderType = ShaderType::Compute;
			}
			else {
				ED_ASSERT_CONTEXT(Shader, 0, "Unsuported shader type");
				currentShaderType = ShaderType::None;
			}
		}
		else {
			source += line + "\n";
		}
	}

	if (currentShaderType != ShaderType::None) {
		shader->SetShaderCode(currentShaderType, source);
	}

	shader->LinkProgram();

	return shader;
}

std::shared_ptr<Framebuffer> RenderingHelper::CreateFramebuffer(uint32_t width, uint32_t height)
{
	return std::make_shared<OpenGLFramebuffer>(width, height);
}

std::shared_ptr<CubeFramebuffer> RenderingHelper::CreateCubeFramebuffer(uint32_t size)
{
	return std::make_shared<OpenGLCubeFramebuffer>(size);
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(const std::shared_ptr<Texture2DDescriptor>& descriptor)
{
	return std::make_shared<OpenGLTexture2D>(descriptor); // TODO: Add logic for API ;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(Texture2DImportParameters& parameters, Texture2DData& data, const std::string& name)
{
	return CreateTexture2D(std::move(parameters), std::move(data), name);
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(Texture2DImportParameters&& parameters, Texture2DData&& data, const std::string& name)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>();
	descriptor->ImportParameters = std::move(parameters);
	descriptor->Data = std::move(data);
	descriptor->AssetName = name;

	std::shared_ptr<Texture2D> texture = std::make_shared<OpenGLTexture2D>(descriptor); // TODO: Add logic for API 

// 	std::shared_ptr<AssetManager> manager = Engine::Get().GetManager<AssetManager>();
// 	manager->RegisterTexture(texture); // TODO: make it register descriptors

	return texture;
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(const std::shared_ptr<CubeTextureDescriptor>& descriptor)
{
	return std::make_shared<OpenGLCubeTexture>(descriptor);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(CubeTextureImportParameters& parameters, CubeTextureData& data, const std::string& name)
{
	return CreateCubeTexture(std::move(parameters), std::move(data), name);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(CubeTextureImportParameters&& parameters, CubeTextureData&& data, const std::string& name)
{
	std::shared_ptr<CubeTextureDescriptor> descriptor = std::make_shared<CubeTextureDescriptor>();
	descriptor->ImportParameters = std::move(parameters);
	descriptor->Data = std::move(data);

	std::shared_ptr<CubeTexture> texture = std::make_shared<OpenGLCubeTexture>(descriptor); // TODO: Add logic for API 
// 
// 	manager->RegisterTexture(texture); // TODO: make it register descriptors
// 	std::shared_ptr<AssetManager> manager = Engine::Get().GetManager<AssetManager>();

	return texture;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateBloomIntermediateTexture()
{
	Texture2DImportParameters parameters;
	parameters.Format = PixelFormat::R11G11B10F;
	parameters.WrapS = WrapMode::ClampToEdge;
	parameters.WrapT = WrapMode::ClampToEdge;

	Texture2DData data;
	data.Data = nullptr;
	data.Width = 1;
	data.Height = 1;

	return RenderingHelper::CreateTexture2D(parameters, data, "Bloom downscale iteration texture");
}

Texture2DImportParameters RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(const std::string& path)
{
	Texture2DImportParameters parameters;
	parameters.Path = path;
	parameters.WrapS = WrapMode::Repeat;
	parameters.WrapT = WrapMode::Repeat;
	parameters.Format = PixelFormat::SRGBA8F;
	parameters.Filtering = FilteringMode::Linear;
	parameters.GenerateMipMaps = true;
	return parameters;
}

Texture2DImportParameters RenderingHelper::GetDefaultNormalTexture2DImportParameters(const std::string& path)
{
	Texture2DImportParameters parameters;
	parameters.Path = path;
	parameters.WrapS = WrapMode::Repeat;
	parameters.WrapT = WrapMode::Repeat;
	parameters.Format = PixelFormat::RGBA8F;
	parameters.Filtering = FilteringMode::Linear;
	return parameters;
}

Texture2DImportParameters RenderingHelper::GetDefaultMetalicTexture2DImportParameters(const std::string& path)
{
	Texture2DImportParameters parameters;
	parameters.Path = path;
	parameters.WrapS = WrapMode::Repeat;
	parameters.WrapT = WrapMode::Repeat;
	parameters.Format = PixelFormat::R8F;
	parameters.Filtering = FilteringMode::Linear;
	return parameters;
}

Texture2DImportParameters RenderingHelper::GetDefaultRoughnessTexture2DImportParameters(const std::string& path)
{
	Texture2DImportParameters parameters;
	parameters.Path = path;
	parameters.WrapS = WrapMode::Repeat;
	parameters.WrapT = WrapMode::Repeat;
	parameters.Format = PixelFormat::R8F;
	parameters.Filtering = FilteringMode::Linear;
	return parameters;
}


std::shared_ptr<Texture2D> RenderingHelper::GetWhiteTexture()
{
	static bool isInitialized = false;
	static std::shared_ptr<Texture2D> texutre;

	if (!isInitialized)
	{
		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::Repeat;
		parameters.WrapT = WrapMode::Repeat;
		parameters.Format = PixelFormat::RGB8F;
		parameters.Filtering = FilteringMode::Linear;

		Texture2DData data;
		data.Width = 1;
		data.Height = 1;
		data.Data = (uint8_t*) std::malloc(sizeof(uint32_t));
		data.Data[0] = 255;
		data.Data[1] = 255;
		data.Data[2] = 255;

		texutre = CreateTexture2D(std::move(parameters), std::move(data), "white texture");

		isInitialized = true;
	}

	return texutre;
}

std::shared_ptr<Texture2D> RenderingHelper::GetBlackTexture()
{
	static bool isInitialized = false;
	static std::shared_ptr<Texture2D> texutre;

	if (!isInitialized)
	{
		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::Repeat;
		parameters.WrapT = WrapMode::Repeat;
		parameters.Format = PixelFormat::RGB8F;
		parameters.Filtering = FilteringMode::Linear;

		Texture2DData data;
		data.Width = 1;
		data.Height = 1;
		data.Data = (uint8_t*)std::malloc(sizeof(uint32_t));
		data.Data[0] = 0;
		data.Data[1] = 0;
		data.Data[2] = 0;

		texutre = CreateTexture2D(std::move(parameters), std::move(data), "black texture");

		isInitialized = true;
	}

	return texutre;
}
