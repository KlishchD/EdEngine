#include "RenderingHelper.h"
#include "Platform/Rendering/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLVertexBuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLIndexBuffer.h"
#include "Platform/Rendering/OpenGL/Buffers/OpenGLUniformBuffer.h"
#include "Platform/Rendering/OpenGL/OpenGLRenderingContext.h"
#include "Platform/Rendering/OpenGL/OpenGLShader.h"
#include "Platform/Rendering/OpenGL/OpenGLWindow.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLCubeTexture.h"
#include "Platform/Rendering/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "Core/Rendering/RenderGraph.h"
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

std::shared_ptr<UniformBuffer> RenderingHelper::CreateUniformBuffer()
{
	return std::make_shared<OpenGLUniformBuffer>();
}

std::shared_ptr<UniformBuffer> RenderingHelper::CreateUniformBuffer(void* data, uint32_t size, BufferUsage usage)
{
	std::shared_ptr<UniformBuffer> buffer = CreateUniformBuffer();
	buffer->SetData(data, size, usage);
	return buffer;
}

std::shared_ptr<Shader> RenderingHelper::CreateShader(const std::string& path)
{
	std::string source;
	ShaderType currentShaderType = ShaderType::None;
	
	std::string fullPath = FilesHelper::ContentFolderPath + "\\" + path;

	std::fstream file(fullPath, std::ios_base::in);
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
				ED_ASSERT_CONTEXT(Shader, 0, "Unsupported shader type");
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

std::shared_ptr<Texture> RenderingHelper::CreateRenderTarget(const RenderTargetSpecification& specification, TextureType textureType)
{
	std::shared_ptr<TextureImportParameters> parameters;
	std::shared_ptr<Texture> target;

	switch (textureType)
	{
	case TextureType::Texture2D:
	{
		target = CreateTexture2D(specification.Name);
		parameters = GetRenderTargetTexture2DImportParameters(specification.Type);
	}
	break;
	case TextureType::CubeTexture:
	{
		target = CreateCubeTexture(specification.Name);
		parameters = GetRenderTargetCubeTextureImportParameters(specification.Type);
	}
	break;
	case TextureType::Texture2DArray:
	{
		target = CreateTexture2DArray(specification.Name);
		parameters = GetRenderTargetTexture2DArrayImportParameters(specification.Type);
	}
	break;
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Unsupported texutre type")
		break;
	}

	target->SetImportParameters(parameters);
	target->ResetState();

	target->Initialize();

	return target;
}

std::shared_ptr<Framebuffer> RenderingHelper::CreateFramebuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t depth, const std::vector<RenderTargetSpecification>& renderTargets, TextureType textureType)
{
	FramebufferSpecification specification;
	specification.Name = name;
	specification.Size = { width, height, depth };
	specification.RenderTargets = renderTargets;
	specification.TextureType = textureType;

	return CreateFramebuffer(specification);
}

std::shared_ptr<Framebuffer> RenderingHelper::CreateFramebuffer(const FramebufferSpecification& specification)
{
	std::shared_ptr<Framebuffer> framebuffer = std::make_shared<OpenGLFramebuffer>(specification);

	for (const RenderTargetSpecification& targetSpecification : specification.RenderTargets)
	{
		std::shared_ptr<Texture> attachment = CreateRenderTarget(targetSpecification, specification.TextureType);
		framebuffer->AddAttachment(attachment);
	}

	return framebuffer;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(const std::string& name)
{
	return std::make_shared<OpenGLTexture2D>(name);
}

std::shared_ptr<Texture2D> RenderingHelper::CreateTexture2D(const std::string& name, std::shared_ptr<Texture2DImportParameters> parameters, Texture2DData&& data)
{
	std::shared_ptr<Texture2D> texture = CreateTexture2D(name);

	texture->SetImportParameters(parameters);

	texture->SetWrapS(parameters->WrapS);
	texture->SetWrapT(parameters->WrapT);

	texture->SetPixelFormat(parameters->Format);
	texture->SetFilteringMode(parameters->Filtering);

	texture->SetData(std::move(data));

	texture->Initialize();

	return texture;
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(const std::string& name)
{
	return std::make_shared<OpenGLCubeTexture>(name);
}

std::shared_ptr<CubeTexture> RenderingHelper::CreateCubeTexture(const std::string& name, std::shared_ptr<CubeTextureImportParameters> parameters, CubeTextureData&& data)
{
	std::shared_ptr<CubeTexture> texture = CreateCubeTexture(name);
	
	texture->SetImportParameters(parameters);

	texture->SetWrapS(parameters->WrapS);
	texture->SetWrapT(parameters->WrapT);
	texture->SetWrapR(parameters->WrapR);

	texture->SetPixelFormat(parameters->Format);
	texture->SetFilteringMode(parameters->Filtering);

	texture->SetData(std::move(data));

	texture->Initialize();

	return texture;
}

std::shared_ptr<Texture2DArray> RenderingHelper::CreateTexture2DArray(const std::string& name)
{
	return std::make_shared<OpenGLTexture2DArray>(name);
}

std::shared_ptr<Texture2DArray> RenderingHelper::CreateTexture2DArray(const std::string& name, std::shared_ptr<Texture2DArrayImportParameters> parameters, Texture2DArrayData&& data)
{
	std::shared_ptr<Texture2DArray> texture = CreateTexture2DArray(name);

	texture->SetImportParameters(parameters);

	texture->SetWrapS(parameters->WrapS);
	texture->SetWrapT(parameters->WrapT);

	texture->SetPixelFormat(parameters->Format);
	texture->SetFilteringMode(parameters->Filtering);

	texture->SetData(std::move(data));

	texture->Initialize();

	return texture;
}

std::shared_ptr<Texture2D> RenderingHelper::CreateBloomIntermediateTexture()
{
	std::shared_ptr<Texture2D> texture = CreateTexture2D("Bloom downscale iteration texture");

	texture->SetPixelFormat(PixelFormat::R11G11B10F);
	texture->SetWrapS(WrapMode::ClampToEdge);
	texture->SetWrapS(WrapMode::ClampToEdge);

	texture->Initialize();

	return texture;
}

std::shared_ptr<Texture2DImportParameters> RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(const std::string& path)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::make_shared<Texture2DImportParameters>();
	parameters->Path = path;
	parameters->WrapS = WrapMode::Repeat;
	parameters->WrapT = WrapMode::Repeat;
	parameters->Format = PixelFormat::SRGBA8F;
	parameters->Filtering = FilteringMode::Linear;
	parameters->GenerateMipMaps = true;
	return parameters;
}

std::shared_ptr<Texture2DImportParameters> RenderingHelper::GetDefaultNormalTexture2DImportParameters(const std::string& path)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::make_shared<Texture2DImportParameters>();
	parameters->Path = path;
	parameters->WrapS = WrapMode::Repeat;
	parameters->WrapT = WrapMode::Repeat;
	parameters->Format = PixelFormat::RGBA8F;
	parameters->Filtering = FilteringMode::Linear;
	return parameters;
}

std::shared_ptr<Texture2DImportParameters> RenderingHelper::GetDefaultMetalicTexture2DImportParameters(const std::string& path)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::make_shared<Texture2DImportParameters>();
	parameters->Path = path;
	parameters->WrapS = WrapMode::Repeat;
	parameters->WrapT = WrapMode::Repeat;
	parameters->Format = PixelFormat::R8F;
	parameters->Filtering = FilteringMode::Linear;
	return parameters;
}

std::shared_ptr<Texture2DImportParameters> RenderingHelper::GetDefaultRoughnessTexture2DImportParameters(const std::string& path)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::make_shared<Texture2DImportParameters>();
	parameters->Path = path;
	parameters->WrapS = WrapMode::Repeat;
	parameters->WrapT = WrapMode::Repeat;
	parameters->Format = PixelFormat::R8F;
	parameters->Filtering = FilteringMode::Linear;
	return parameters;
}

std::shared_ptr<Texture2D> RenderingHelper::ImportBaseColorTexture(const std::string& path)
{
	std::shared_ptr<AssetManager> assetManager = Engine::Get().GetManager<AssetManager>();
	return assetManager->ImportAsset<Texture2D>(AssetType::Texture2D, RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(FilesHelper::ContentFolderPath + "\\" + path));
}

std::shared_ptr<Texture2D> RenderingHelper::ImportNormalTexture(const std::string& path)
{
	std::shared_ptr<AssetManager> assetManager = Engine::Get().GetManager<AssetManager>();
	return assetManager->ImportAsset<Texture2D>(AssetType::Texture2D, RenderingHelper::GetDefaultNormalTexture2DImportParameters(FilesHelper::ContentFolderPath + "\\" + path));
}

std::shared_ptr<Texture2D> RenderingHelper::ImportMetalicTexture(const std::string& path)
{
	std::shared_ptr<AssetManager> assetManager = Engine::Get().GetManager<AssetManager>();
	return assetManager->ImportAsset<Texture2D>(AssetType::Texture2D, RenderingHelper::GetDefaultMetalicTexture2DImportParameters(FilesHelper::ContentFolderPath + "\\" + path));
}

std::shared_ptr<Texture2D> RenderingHelper::ImportRoughnessTexture(const std::string& path)
{
	std::shared_ptr<AssetManager> assetManager = Engine::Get().GetManager<AssetManager>();
	return assetManager->ImportAsset<Texture2D>(AssetType::Texture2D, RenderingHelper::GetDefaultRoughnessTexture2DImportParameters(FilesHelper::ContentFolderPath + "\\" + path));
}

std::shared_ptr<Texture2D> RenderingHelper::GetWhiteTexture()
{
	static std::shared_ptr<Texture2D> texutre = CreateTexture2D("White texture");

	if (!texutre->IsInitialized())
	{
		texutre->SetWrapS(WrapMode::Repeat);
		texutre->SetWrapT(WrapMode::Repeat);

		texutre->SetPixelFormat(PixelFormat::RGB8F);

		texutre->SetFilteringMode(FilteringMode::Linear);

		static uint8_t whiteData[3] = { 255, 255, 255 };
		Texture2DData data(1, 1, &whiteData[0], sizeof(uint32_t), false);

		texutre->SetData(std::move(data));

		texutre->Initialize();
	}

	return texutre;
}

bool RenderingHelper::IsLightMeshVisible(const std::vector<glm::vec3>& vertices, const Transform& transform, const Camera& camera)
{
	glm::mat4 projectionViewModelMatrix = camera.GetProjectionView() * transform.GetMatrix();

	glm::vec3 leftBottonCorner(std::numeric_limits<float>::max());
	glm::vec3 rightTopCorner(std::numeric_limits<float>::min());

	for (const glm::vec3& point : vertices)
	{
		glm::vec4 transformed = projectionViewModelMatrix * glm::vec4(point, 1.0f);
		transformed /= transformed.w;

		leftBottonCorner.x = glm::min(leftBottonCorner.x, transformed.x);
		leftBottonCorner.y = glm::min(leftBottonCorner.y, transformed.y);
		leftBottonCorner.z = glm::min(leftBottonCorner.z, transformed.z);

		rightTopCorner.x = glm::max(rightTopCorner.x, transformed.x);
		rightTopCorner.y = glm::max(rightTopCorner.y, transformed.y);
		rightTopCorner.z = glm::max(rightTopCorner.z, transformed.z);
	}

	bool xChangesSign = leftBottonCorner.x * rightTopCorner.x < 0;
	bool yChangesSign = leftBottonCorner.y * rightTopCorner.y < 0;
	bool zChangesSign = leftBottonCorner.z * rightTopCorner.z < 0;

	bool xInViewRange = (leftBottonCorner.x >= -1.0f && leftBottonCorner.x <= 1.0f) || (rightTopCorner.x >= -1.0f && rightTopCorner.x <= 1.0f);
	bool yInViewRange = (leftBottonCorner.y >= -1.0f && leftBottonCorner.y <= 1.0f) || (rightTopCorner.y >= -1.0f && rightTopCorner.y <= 1.0f);
	bool zInViewRange = (leftBottonCorner.z >= 0.0f && leftBottonCorner.z <= 1.0f) || (rightTopCorner.z >= 0.0f && rightTopCorner.z <= 1.0f);

	return (xChangesSign && yChangesSign && zChangesSign) || (xInViewRange && yInViewRange && zInViewRange) ||
		(xChangesSign && yChangesSign && zInViewRange) || (yChangesSign && zChangesSign && xInViewRange) || (xChangesSign && zChangesSign && yInViewRange) ||
		(xChangesSign && yInViewRange && zInViewRange) || (yChangesSign && xInViewRange && zInViewRange) || (zChangesSign && yInViewRange && xInViewRange);
}

std::shared_ptr<Texture2DImportParameters> RenderingHelper::GetRenderTargetTexture2DImportParameters(FramebufferAttachmentType type)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::make_shared<Texture2DImportParameters>();

	switch (type)
	{
	case FramebufferAttachmentType::Color:  
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA8F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Color16:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA16F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Depth:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::Depth;
		parameters->Filtering = FilteringMode::Nearest;
	}
	break;
	case FramebufferAttachmentType::DepthStencil:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::DepthStencil;
		parameters->Filtering = FilteringMode::Nearest;
	}
	break;
	case FramebufferAttachmentType::Position:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB32F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Direction:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB16F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Velocity:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RG16F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Distance:
	{
		parameters->WrapS = WrapMode::MirroredRepeat;
		parameters->WrapT = WrapMode::MirroredRepeat;
		parameters->Format = PixelFormat::R8F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	case FramebufferAttachmentType::Bloom:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::R11G11B10F;
		parameters->Filtering = FilteringMode::Linear;
	}
	break;
	default:
		ED_ASSERT_CONTEXT(RenderingHelper, 0, "Unsupported render target type")
		break;
	}
	
	return parameters;
}

std::shared_ptr<CubeTextureImportParameters> RenderingHelper::GetRenderTargetCubeTextureImportParameters(FramebufferAttachmentType type)
{
	std::shared_ptr<CubeTextureImportParameters> parameters = std::make_shared<CubeTextureImportParameters>();

	switch (type)
	{
	case FramebufferAttachmentType::Color:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA8F;
		parameters->Filtering = FilteringMode::Linear;
		parameters->WrapR = WrapMode::ClampToEdge;
	}; break;
	case FramebufferAttachmentType::Color16:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA16F;
		parameters->Filtering = FilteringMode::Linear;
		parameters->WrapR = WrapMode::ClampToEdge;
	}; break;
	case FramebufferAttachmentType::Depth:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::Depth;
		parameters->Filtering = FilteringMode::Nearest;
		parameters->WrapR = WrapMode::Repeat;
	}; break;
	case FramebufferAttachmentType::DepthStencil:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::DepthStencil;
		parameters->Filtering = FilteringMode::Nearest;
		parameters->WrapR = WrapMode::Repeat;
	}; break;
	case FramebufferAttachmentType::Direction:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB8F;
		parameters->Filtering = FilteringMode::Linear;
		parameters->WrapR = WrapMode::Repeat;
	}; break;
	case FramebufferAttachmentType::Position:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB32F;
		parameters->Filtering = FilteringMode::Linear;
		parameters->WrapR = WrapMode::Repeat;
	}; break; // TODO: check for artifacts :)
	case FramebufferAttachmentType::Distance:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::R8F;
		parameters->Filtering = FilteringMode::Linear;
		parameters->WrapR = WrapMode::Repeat;
	}; break; // TODO: check for artifacts :)
	default:
		ED_ASSERT_CONTEXT(RenderingHelper, 0, "Unsupported render target type")
		break;
	}

	return parameters;
}

std::shared_ptr<Texture2DArrayImportParameters> RenderingHelper::GetRenderTargetTexture2DArrayImportParameters(FramebufferAttachmentType type)
{
	std::shared_ptr<Texture2DArrayImportParameters> parameters = std::make_shared<Texture2DArrayImportParameters>();

	switch (type)
	{
	case FramebufferAttachmentType::Color:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA8F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Color16:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::RGBA16F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Depth:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::Depth;
		parameters->Filtering = FilteringMode::Nearest;
	}; break;
	case FramebufferAttachmentType::DepthStencil:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::DepthStencil;
		parameters->Filtering = FilteringMode::Nearest;
	}; break;
	case FramebufferAttachmentType::Position:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB32F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Direction:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RGB16F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Velocity:
	{
		parameters->WrapS = WrapMode::Repeat;
		parameters->WrapT = WrapMode::Repeat;
		parameters->Format = PixelFormat::RG16F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Distance:
	{
		parameters->WrapS = WrapMode::MirroredRepeat;
		parameters->WrapT = WrapMode::MirroredRepeat;
		parameters->Format = PixelFormat::R8F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	case FramebufferAttachmentType::Bloom:
	{
		parameters->WrapS = WrapMode::ClampToEdge;
		parameters->WrapT = WrapMode::ClampToEdge;
		parameters->Format = PixelFormat::R11G11B10F;
		parameters->Filtering = FilteringMode::Linear;
	}; break;
	default:
		ED_ASSERT_CONTEXT(RenderingHelper, 0, "Unsupported render target type")
		break;
	}

	return parameters;
}
