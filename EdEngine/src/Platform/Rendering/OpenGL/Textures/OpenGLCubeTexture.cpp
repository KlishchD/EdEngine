#include "OpenGLCubeTexture.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLCubeTexture::OpenGLCubeTexture(const std::shared_ptr<CubeTextureDescriptor>& descriptor): CubeTexture(descriptor)
{
	Initialize(descriptor->GetImportParameters(), descriptor->GetData());
}

void OpenGLCubeTexture::Resize(uint32_t size)
{
	if (GetSize() != size)
	{
		CubeTextureData& data = (CubeTextureData&)GetData();
		data.Size = size;

		const CubeTextureImportParameters& parameters = (const CubeTextureImportParameters&)GetImportParameters();

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

void OpenGLCubeTexture::Initialize(TextureImportParameters* inParameters, TextureData* inData)
{
	const CubeTextureImportParameters& parameters = (const CubeTextureImportParameters&) *inParameters;
	const CubeTextureData& data = (const CubeTextureData&) *inData;

	glGenTextures(1, &m_Id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(parameters.WrapS));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(parameters.WrapT));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLTypes::ConvertWrapMode(parameters.WrapR));

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));

	for (int32_t i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.Size, data.Size, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.Data[i]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
