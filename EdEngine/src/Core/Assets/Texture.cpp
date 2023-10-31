#include "Texture.h"
#include "Sampler.h"

Texture::Texture()
{
}

void Texture::Bind(uint32_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(m_TextureType, m_Id);
}

void Texture::Unbind()
{
	glBindTexture(m_TextureType, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_Id);
}

uint32_t Texture::GetID() const
{
	return m_Id;
}
