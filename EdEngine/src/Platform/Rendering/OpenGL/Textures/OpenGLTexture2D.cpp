#include "OpenGLTexture2D.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"
#include "Core/Rendering/EdRendering.h"

OpenGLTexture2D::OpenGLTexture2D(const std::string& name): Super(name)
{
	glGenTextures(1, &m_Id);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLTexture2D::Initialize()
{
	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode, m_bMipMapsEnabled));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));
	
	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetWidth(), m_Data.GetHeight(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());
	
	if (m_bMipMapsEnabled)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	m_bIsInitialized = true;
}

void OpenGLTexture2D::RefreshData()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetWidth(), m_Data.GetHeight(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());

		if (m_bMipMapsEnabled)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void OpenGLTexture2D::GenerateMipMaps()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void OpenGLTexture2D::DeleteMipMaps()
{
	// TODO: Deallocate them :)
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void OpenGLTexture2D::RefreshParameters()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode, m_bMipMapsEnabled));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
