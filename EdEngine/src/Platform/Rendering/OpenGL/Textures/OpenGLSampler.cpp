#include "OpenGLSampler.h"
#include "Core/Rendering/EdRendering.h"

OpenGLSampler::OpenGLSampler()
{
	glGenSamplers(1, &m_Id);

	glSamplerParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void OpenGLSampler::EnbaleAnisotrophicFiltering(float samples)
{
	glSamplerParameterfv(m_Id, GL_TEXTURE_MAX_ANISOTROPY, &samples);
}

OpenGLSampler::~OpenGLSampler()
{
	glDeleteSamplers(1, &m_Id);
}
