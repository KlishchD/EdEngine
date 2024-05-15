#include "OpenGLCubeTexture.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLCubeTexture::OpenGLCubeTexture(const std::string& name) : Super(name)
{
	glGenTextures(1, &m_Id);
}

OpenGLCubeTexture::~OpenGLCubeTexture()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLCubeTexture::Initialize()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLTypes::ConvertWrapMode(m_WrapR));

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

	for (int32_t i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetSize(), m_Data.GetSize(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_bIsInitialized = true;
}

void OpenGLCubeTexture::RefreshData()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

		for (int32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetSize(), m_Data.GetSize(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

void OpenGLCubeTexture::RefreshParameters()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLTypes::ConvertWrapMode(m_WrapR));

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
