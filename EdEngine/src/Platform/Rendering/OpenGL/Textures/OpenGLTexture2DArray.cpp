#include "OpenGLTexture2DArray.h"
#include "Core/Rendering/EdRendering.h"
#include "Platform/Rendering/OpenGL/OpenGLTypes.h"

OpenGLTexture2DArray::OpenGLTexture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descpriptor) : Texture2DArray(descpriptor)
{
	glGenTextures(1, &m_Id);
	Initialize();
}

void OpenGLTexture2DArray::Resize(uint32_t width, uint32_t height, uint32_t depth)
{
	std::shared_ptr<Texture2DArrayDescriptor> descriptor = GetDescriptor<Texture2DArrayDescriptor>();
	Texture2DArrayImportParameters& parameters = descriptor->ImportParameters;
	Texture2DArrayData& data = descriptor->Data;

	if (data.GetWidth() != width || data.GetHeight() != height || data.GetDepth() != depth)
	{
		data.SetWidth(width);
		data.SetHeight(height);
		data.SetDepth(depth);

		glBindTexture(GL_TEXTURE_2D_ARRAY, m_Id);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), width, height, depth, 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), nullptr);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
}

void OpenGLTexture2DArray::Initialize()
{
	std::shared_ptr<Texture2DArrayDescriptor> descriptor = GetDescriptor<Texture2DArrayDescriptor>();
	Texture2DArrayImportParameters& parameters = descriptor->ImportParameters;
	Texture2DArrayData& data = descriptor->Data;

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Id);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, OpenGLTypes::ConvertWrapMode(parameters.WrapS));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, OpenGLTypes::ConvertWrapMode(parameters.WrapT));

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, OpenGLTypes::ConvertFilteringMode(parameters.Filtering));

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, OpenGLTypes::ConvertPixelFormat(parameters.Format), data.GetWidth(), data.GetHeight(), data.GetDepth(), 0, OpenGLTypes::ConvertPixelExternalFormat(parameters.Format), OpenGLTypes::ConvertDataType(parameters.Format), data.GetData());

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
