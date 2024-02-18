#include "OpenGLCubeTexture.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLCubeTexture::OpenGLCubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor): CubeTexture(descriptor)
{
	glGenTextures(1, &m_Id);
	Initialize();
}

void OpenGLCubeTexture::Resize(uint32_t size)
{
	std::shared_ptr<CubeTextureDescriptor> descriptor = GetDescriptor<CubeTextureDescriptor>();
	CubeTextureImportParameters& parameters = descriptor->ImportParameters;
	CubeTextureData& data = descriptor->Data;

	if (data.GetSize() != size)
	{
		data.SetSize(size);

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

		for (int32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), size, size, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), nullptr);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

TextureType OpenGLCubeTexture::GetType() const
{
	return TextureType::CubeTexture;
}

OpenGLCubeTexture::~OpenGLCubeTexture()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLCubeTexture::Initialize()
{
	std::shared_ptr<CubeTextureDescriptor> descriptor = GetDescriptor<CubeTextureDescriptor>();
	CubeTextureImportParameters& parameters = descriptor->ImportParameters;
	CubeTextureData& data = descriptor->Data;

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(parameters.WrapS));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(parameters.WrapT));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLTypes::ConvertWrapMode(parameters.WrapR));

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));

	for (int32_t i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.GetSize(), data.GetSize(), 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.GetData());
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
