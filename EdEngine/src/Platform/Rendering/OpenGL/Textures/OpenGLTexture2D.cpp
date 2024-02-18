#include "OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"
#include "Core/Rendering/EdRendering.h"

OpenGLTexture2D::OpenGLTexture2D(std::shared_ptr<Texture2DDescriptor> descriptor): Texture2D(descriptor)
{
	glGenTextures(1, &m_Id);
	Initialize();
}

void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = GetDescriptor<Texture2DDescriptor>();
	Texture2DImportParameters& parameters = descriptor->ImportParameters;
	Texture2DData& data = descriptor->Data;

	if (data.GetWidth() != width || data.GetHeight() != height)
	{
		data.SetWidth(width);
		data.SetHeight(height);

		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), width, height, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

TextureType OpenGLTexture2D::GetType() const
{
	return TextureType::Texture2D;
}

void OpenGLTexture2D::SetData(const Texture2DData& inData)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = GetDescriptor<Texture2DDescriptor>();
	descriptor->Data = inData;

	Texture2DImportParameters& parameters = descriptor->ImportParameters;
	Texture2DData& data = descriptor->Data;

	MarkDirty();

	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.GetWidth(), data.GetHeight(), 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.GetData());

	if (parameters.GenerateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture2D::SetData(Texture2DData&& inData)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = GetDescriptor<Texture2DDescriptor>();
	descriptor->Data = std::move(inData);

	Texture2DImportParameters& parameters = descriptor->ImportParameters;
	Texture2DData& data = descriptor->Data;

	MarkDirty();

	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.GetWidth(), data.GetHeight(), 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.GetData());

	if (parameters.GenerateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLTexture2D::Initialize()
{
	std::shared_ptr<Texture2DDescriptor> descriptor = GetDescriptor<Texture2DDescriptor>();
	Texture2DImportParameters& parameters = descriptor->ImportParameters;
	Texture2DData& data = descriptor->Data;

	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(parameters.WrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(parameters.WrapT));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering, parameters.GenerateMipMaps));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));

	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.GetWidth(), data.GetHeight(), 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.GetData());
	if (parameters.GenerateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}
