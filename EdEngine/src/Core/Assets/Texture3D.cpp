#include "Texture3D.h"

Texture3D::Texture3D(const std::shared_ptr<Texture3DDescriptor>& descriptor)
{
    SetDescriptor(descriptor);
    Init(descriptor->ImportParameters, descriptor->TextureData);
}

Texture3D::Texture3D(const Texture3DImportParameters& parameters, const Texture3DData& data)
{
    std::shared_ptr<Texture3DDescriptor> descriptor = std::make_shared<Texture3DDescriptor>();
    descriptor->ImportParameters = parameters;
    descriptor->TextureData = data;

    SetDescriptor(descriptor);
    Init(parameters, data);
}

void Texture3D::Resize(uint32_t width, uint32_t height)
{
    int32_t size = std::max(width, height);
    if (m_Width != size) {
        m_Width = size;
        m_Height = size;

        Texture3DImportParameters& parameters = std::static_pointer_cast<Texture3DDescriptor>(m_Descriptor)->ImportParameters;
	
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
        for (int32_t i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, parameters.InternalFormat, size, size, 0, parameters.ExternalFormat, parameters.DataType, nullptr);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}

void Texture3D::Init(const Texture3DImportParameters& parameters, const Texture3DData& data)
{
    m_TextureType = GL_TEXTURE_CUBE_MAP;
    m_Width = data.Size;
    m_Height = data.Size;
    
    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, parameters.WrapS);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, parameters.WrapT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, parameters.WrapR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, parameters.Filtering);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, parameters.Filtering);

    for (int32_t i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, parameters.InternalFormat, data.Size, data.Size, 0, parameters.ExternalFormat, parameters.DataType, data.Data[i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
