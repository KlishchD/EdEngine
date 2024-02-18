#include "RenderingHelper.h"
#include "Platform/Rendering/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLVertexBuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLIndexBuffer.h"
#include "Platform/Rendering/OpenGL/OpenGLRenderingContext.h"
#include "Platform/Rendering/OpenGL/OpenGLShader.h"
#include "Platform/Rendering/OpenGL/OpenGLWindow.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLCubeTexture.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2DArray.h"
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

std::shared_ptr<Texture> RenderingHelper::CreateDefaultFramebufferAttachment(FramebufferAttachmentType type, TextureType textureType)
{
	switch (textureType)
	{
	case TextureType::Texture2D:
	{
		Texture2DImportParameters parameters;

		switch (type)
		{
		case FramebufferAttachmentType::Color:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA8F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Color16:      parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA16F,      FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Depth:        parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::Depth,        FilteringMode::Nearest }; break;
		case FramebufferAttachmentType::DepthStencil: parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::DepthStencil, FilteringMode::Nearest }; break;
		case FramebufferAttachmentType::Position:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB32F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Direction:    parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB16F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Velocity:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RG16F,        FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Distance:     parameters = { "", WrapMode::MirroredRepeat, WrapMode::MirroredRepeat, PixelFormat::R8F,          FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Bloom:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::R11G11B10F,   FilteringMode::Linear }; break;
		default:
			ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported framebuffer attachment type")
				break;
		}

		Texture2DData data(1, 1);

		return CreateTexture2D(std::move(parameters), std::move(data), "Framebuffer attachment");
	}
	case TextureType::CubeTexture:
	{
		CubeTextureImportParameters parameters;

		switch (type)
		{
		case FramebufferAttachmentType::Color:        parameters = { "", WrapMode::ClampToEdge, WrapMode::ClampToEdge, PixelFormat::RGBA8F,        FilteringMode::Linear,  WrapMode::ClampToEdge }; break;
		case FramebufferAttachmentType::Color16:      parameters = { "", WrapMode::ClampToEdge, WrapMode::ClampToEdge, PixelFormat::RGBA16F,       FilteringMode::Linear,  WrapMode::ClampToEdge }; break;
		case FramebufferAttachmentType::Depth:        parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::Depth,         FilteringMode::Nearest, WrapMode::Repeat }; break;
		case FramebufferAttachmentType::DepthStencil: parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::DepthStencil,  FilteringMode::Nearest, WrapMode::Repeat }; break;
		case FramebufferAttachmentType::Direction:    parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::RGB8F,         FilteringMode::Linear,  WrapMode::Repeat }; break;
		case FramebufferAttachmentType::Position:     parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::RGB32F,        FilteringMode::Linear,  WrapMode::Repeat }; break; // TODO: check for artifacts :)
		case FramebufferAttachmentType::Distance:     parameters = { "", WrapMode::Repeat,      WrapMode::Repeat,      PixelFormat::R8F,           FilteringMode::Linear,  WrapMode::Repeat }; break; // TODO: check for artifacts :)
		default:
			ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported cube framebuffer attachment type");
			break;
		}

		CubeTextureData data(1);

		return CreateCubeTexture(std::move(parameters), std::move(data), "Cubeframebuffer attachment");
	}
	case TextureType::Texture2DArray:
	{
		Texture2DArrayImportParameters parameters;

		switch (type)
		{
		case FramebufferAttachmentType::Color:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA8F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Color16:      parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::RGBA16F,      FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Depth:        parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::Depth,        FilteringMode::Nearest }; break;
		case FramebufferAttachmentType::DepthStencil: parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::DepthStencil, FilteringMode::Nearest }; break;
		case FramebufferAttachmentType::Position:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB32F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Direction:    parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RGB16F,       FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Velocity:     parameters = { "", WrapMode::Repeat,         WrapMode::Repeat,         PixelFormat::RG16F,        FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Distance:     parameters = { "", WrapMode::MirroredRepeat, WrapMode::MirroredRepeat, PixelFormat::R8F,          FilteringMode::Linear }; break;
		case FramebufferAttachmentType::Bloom:        parameters = { "", WrapMode::ClampToEdge,    WrapMode::ClampToEdge,    PixelFormat::R11G11B10F,   FilteringMode::Linear }; break;
		default:
			ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported framebuffer attachment type")
			break;
		}

		Texture2DArrayData data(1, 1, 1);

		return CreateTexture2DArray(std::move(parameters), std::move(data), "Framebuffer attachment");
	}
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported texutre type")
		break;
	}
}

std::shared_ptr<Framebuffer> RenderingHelper::CreateFramebuffer(uint32_t width, uint32_t height, uint32_t depth)
{
	return std::make_shared<OpenGLFramebuffer>(width, height, depth);
}

std::shared_ptr<Framebuffer> RenderingHelper::CreateFramebuffer(uint32_t width, uint32_t height, uint32_t depth, const std::vector<FramebufferAttachmentType>& initialAttachments, TextureType textureType)
{
	std::shared_ptr<Framebuffer> framebuffer = std::make_shared<OpenGLFramebuffer>(width, height, depth);

	for (FramebufferAttachmentType type : initialAttachments)
	{
		std::shared_ptr<Texture> attachment = CreateDefaultFramebufferAttachment(type, textureType);
		framebuffer->AddAttacment(attachment);
	}

	return framebuffer;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(std::shared_ptr<Texture2DDescriptor> descriptor)
{
	return std::make_shared<OpenGLTexture2D>(descriptor); // TODO: Add logic for API ;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(const Texture2DImportParameters& parameters, const Texture2DData& data, const std::string& name)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>();
	descriptor->ImportParameters = parameters;
	descriptor->Data = data;
	descriptor->AssetName = name;

	return std::make_shared<OpenGLTexture2D>(descriptor);
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(Texture2DImportParameters&& parameters, Texture2DData&& data, const std::string& name)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>();
	descriptor->ImportParameters = std::move(parameters);
	descriptor->Data = std::move(data);
	descriptor->AssetName = name;

	return std::make_shared<OpenGLTexture2D>(descriptor);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor)
{
	return std::make_shared<OpenGLCubeTexture>(descriptor);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(const CubeTextureImportParameters& parameters, const CubeTextureData& data, const std::string& name)
{
	std::shared_ptr<CubeTextureDescriptor> descriptor = std::make_shared<CubeTextureDescriptor>();
	descriptor->ImportParameters = parameters;
	descriptor->Data = data;

	return std::make_shared<OpenGLCubeTexture>(descriptor);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(CubeTextureImportParameters&& parameters, CubeTextureData&& data, const std::string& name)
{
	std::shared_ptr<CubeTextureDescriptor> descriptor = std::make_shared<CubeTextureDescriptor>();
	descriptor->ImportParameters = std::move(parameters);
	descriptor->Data = std::move(data);

	// TODO: Add logic for API 
	// 	manager->RegisterTexture(texture); // TODO: make it register descriptors

	return std::make_shared<OpenGLCubeTexture>(descriptor);
}

std::shared_ptr<Texture2DArray> RenderingHelper::CreateTexture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descriptor)
{
	return std::make_shared<OpenGLTexture2DArray>(descriptor);
}

std::shared_ptr<Texture2DArray> RenderingHelper::CreateTexture2DArray(const Texture2DArrayImportParameters& parameters, const Texture2DArrayData& data, const std::string& name)
{
	std::shared_ptr<Texture2DArrayDescriptor> descriptor = std::make_shared<Texture2DArrayDescriptor>();
	descriptor->ImportParameters = parameters;
	descriptor->Data = data;
	descriptor->AssetName = name;

	return std::make_shared<OpenGLTexture2DArray>(descriptor);
}

std::shared_ptr<Texture2DArray> RenderingHelper::CreateTexture2DArray(Texture2DArrayImportParameters&& parameters, Texture2DArrayData&& data, const std::string& name)
{
	std::shared_ptr<Texture2DArrayDescriptor> descriptor = std::make_shared<Texture2DArrayDescriptor>();
	descriptor->ImportParameters = std::move(parameters);
	descriptor->Data = std::move(data);
	descriptor->AssetName = name;

	return std::make_shared<OpenGLTexture2DArray>(descriptor);
}

std::shared_ptr<Texture2D> RenderingHelper::CreateBloomIntermediateTexture()
{
	Texture2DImportParameters parameters;
	parameters.Format = PixelFormat::R11G11B10F;
	parameters.WrapS = WrapMode::ClampToEdge;
	parameters.WrapT = WrapMode::ClampToEdge;

	Texture2DData data;

	return RenderingHelper::CreateTexture2D(std::move(parameters), std::move(data), "Bloom downscale iteration texture");
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

		static uint8_t whiteData[3] = { 255, 255, 255 };
		Texture2DData data(1, 1, whiteData, sizeof(uint32_t), false);

		texutre = CreateTexture2D(std::move(parameters), std::move(data), "white texture");

		isInitialized = true;
	}

	return texutre;
}
