#include "OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"
#include "Core/Rendering/EdRendering.h"

OpenGLTexture2D::OpenGLTexture2D(const std::shared_ptr<TextureDescriptor>& descriptor): Texture2D(descriptor)
{
	Initialize(descriptor->GetImportParameters(), descriptor->GetData());
}

void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
{
	if (GetWidth() != width || GetHeight() != height) {
		Texture2DData& Data = (Texture2DData&) GetData();

		Data.Width = width;
		Data.Height = height;

		const Texture2DImportParameters& parameters = (const Texture2DImportParameters&) GetImportParameters();

		glBindTexture(GL_TEXTURE_2D, m_Id);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), width, height, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), nullptr);
		if (parameters.GenerateMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

TextureType OpenGLTexture2D::GetType() const
{
	return TextureType::Texture2D;
}

void OpenGLTexture2D::SetData(const Texture2DData& data)
{
	GetDescriptor<Texture2DDescriptor>()->Data = data;
	MarkDirty();

	Texture::Initialize();
}

void OpenGLTexture2D::SetData(Texture2DData&& data)
{
	GetDescriptor<Texture2DDescriptor>()->Data = std::move(data);
	MarkDirty();

	Texture::Initialize();
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLTexture2D::Initialize(TextureImportParameters* inParameters, TextureData* inData)
{
	const Texture2DImportParameters& parameters = (const Texture2DImportParameters&)*inParameters;
	const Texture2DData& data = (const Texture2DData&)*inData;

	glGenTextures(1, &m_Id);

	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(parameters.WrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(parameters.WrapT));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering, parameters.GenerateMipMaps));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));

	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.Width, data.Height, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.Data);
	if (parameters.GenerateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
