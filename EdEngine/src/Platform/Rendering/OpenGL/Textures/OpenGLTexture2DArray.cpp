#include "OpenGLTexture2DArray.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLTexture2DArray::OpenGLTexture2DArray(const std::string& name) : Super(name)
{
	glGenTextures(1, &m_Id);
}

OpenGLTexture2DArray::~OpenGLTexture2DArray()
{
	glDeleteTextures(1, &m_Id);
}

void OpenGLTexture2DArray::RefreshData()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Id);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetWidth(), m_Data.GetHeight(), m_Data.GetDepth(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}

void OpenGLTexture2DArray::RefreshParameters()
{
	if (m_bIsInitialized)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Id);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}

void OpenGLTexture2DArray::Initialize()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Id);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(m_WrapS));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(m_WrapT));

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(m_FilteringMode));

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, OpenGLTypes::ConvertPixelFormat(m_PixelFormat), m_Data.GetWidth(), m_Data.GetHeight(), m_Data.GetDepth(), 0, OpenGLTypes::ConvertPixelExternalFormat(m_PixelFormat), OpenGLTypes::ConvertDataType(m_PixelFormat), m_Data.GetData());

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	m_bIsInitialized = true;
}
