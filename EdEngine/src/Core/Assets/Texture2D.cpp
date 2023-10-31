#include "Texture2D.h"
#include "Core/Ed.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Texture2DDescriptor)

Texture2DImportParameters Texture2DImportParameters::GetDefaultBaseColorTextureImportParameters(const std::string& path)
{
	return { path, GL_REPEAT, GL_REPEAT, GL_SRGB_ALPHA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
}

Texture2DImportParameters Texture2DImportParameters::GetDefaultNormalTextureImportParameters(const std::string& path)
{
	return { path, GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
}

Texture2DImportParameters Texture2DImportParameters::GetDefaultMetalicTextureImportParameters(const std::string& path)
{
	return { path, GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
}

Texture2DImportParameters Texture2DImportParameters::GetDefaultRoughnessTextureImportParameters(const std::string& path)
{
	return { path, GL_REPEAT, GL_REPEAT, GL_RGBA, GL_RGBA, GL_LINEAR, GL_UNSIGNED_BYTE };
}

Texture2DData::Texture2DData(): Width(0), Height(0), Channels(0), Data(nullptr)
{
}

Texture2DData::Texture2DData(int32_t width, int32_t height, int32_t channels, uint8_t* data): Width(width), Height(height), Channels(channels), Data(data)
{
	
}

Texture2DData::Texture2DData(Texture2DData&& data) noexcept: Width(data.Width), Height(data.Height), Channels(data.Channels), Data(data.Data)
{
	data.Data = nullptr;
}
Texture2D::Texture2D(const std::shared_ptr<Texture2DDescriptor>& descriptor)
{
	SetDescriptor(descriptor);
	Init(descriptor->ImportParameters, descriptor->TextureData);
}

Texture2D::Texture2D(const Texture2DImportParameters& parameters, const Texture2DData& data)
{
	std::shared_ptr<Texture2DDescriptor> descriptor = std::make_shared<Texture2DDescriptor>(); // TODO: register it ;)
	descriptor->ImportParameters = parameters;
	descriptor->TextureData = data;
	SetDescriptor(descriptor);
	
	Init(parameters, data);
}

void Texture2D::Resize(uint32_t width, uint32_t height)
{
	if (m_Width != width || m_Height != height) {
		m_Width = width;
		m_Height = height;

		Texture2DImportParameters& parameters = std::static_pointer_cast<Texture2DDescriptor>(m_Descriptor)->ImportParameters;
		// TODO: Add descriptor size update if needed
	
		glBindTexture(m_TextureType, m_Id);
		glTexImage2D(m_TextureType, 0, parameters.InternalFormat, width, height, 0, parameters.ExternalFormat, parameters.DataType, nullptr);
		glBindTexture(m_TextureType, 0);
	}
}

std::shared_ptr<Texture2D> Texture2D::GetWhiteTexture()
{
	if (!WhiteTexture)
	{
		static uint32_t whiteData[] = {0xffffffff};
		Texture2DData data;
		data.Channels = 4;
		data.Width = 1;
		data.Height = 1;
		data.Data = (uint8_t*)whiteData;
		WhiteTexture = std::make_shared<Texture2D>(Texture2DImportParameters::GetDefaultBaseColorTextureImportParameters(""), data);
	}

	return WhiteTexture;
}

void Texture2D::Init(const Texture2DImportParameters& parameters, const Texture2DData& data)
{
	m_TextureType = GL_TEXTURE_2D;
	m_Width = data.Width;
	m_Height = data.Height;
	
	glGenTextures(1, &m_Id);
	glBindTexture(GL_TEXTURE_2D, m_Id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameters.WrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameters.WrapT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameters.Filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameters.Filtering);

	glTexImage2D(GL_TEXTURE_2D, 0, parameters.InternalFormat, data.Width, data.Height, 0, parameters.ExternalFormat, parameters.DataType, data.Data);

	glBindTexture(GL_TEXTURE_2D, 0);
}
