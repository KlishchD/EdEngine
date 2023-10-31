#include "Sampler.h"

Sampler::Sampler()
{
	glGenSamplers(1, &m_Id);

	glSamplerParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

uint32_t Sampler::GetID() const
{
	return m_Id;
}

void Sampler::Bind(uint32_t slot)
{
	glBindSampler(slot, m_Id);
}

void Sampler::Unbind(uint32_t slot)
{
	glBindSampler(slot, 0);
}

void Sampler::EnbaleAnisotrophicFiltering(const float samples)
{
	glSamplerParameterfv(m_Id, GL_TEXTURE_MAX_ANISOTROPY, &samples);
}

Sampler::~Sampler()
{
	glDeleteSamplers(1, &m_Id);
}
