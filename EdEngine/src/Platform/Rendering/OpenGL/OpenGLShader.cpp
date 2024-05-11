#include "OpenGLShader.h"
#include "Core/Rendering/EdRendering.h"
#include "Core/Macros.h"
#include "OpenGLTypes.h"

OpenGLShader::OpenGLShader()
{
	m_Id = glCreateProgram();
}

void OpenGLShader::SetShaderCode(ShaderType type, const std::string& code)
{
	const uint32_t shaderId = glCreateShader(OpenGLTypes::ConvertShaderType(type));

	const char* shaderSourceC = code.data();
	glShaderSource(shaderId, 1, &shaderSourceC, 0);

	glCompileShader(shaderId);

	int32_t status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char message[1024];
		int32_t messageLength;
		glGetShaderInfoLog(shaderId, 1024, &messageLength, message);
		ED_LOG(Shader, err, "Shader {}: failed to compile shader: {}", m_Id, message);
	}

	glAttachShader(m_Id, shaderId);

	m_ShadersIds.push_back(shaderId);
}

void OpenGLShader::LinkProgram()
{
	glLinkProgram(m_Id);

	glValidateProgram(m_Id);

	int32_t status;
	glGetProgramiv(m_Id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char message[1024];
		int32_t messageLength;
		glGetProgramInfoLog(m_Id, 1024, &messageLength, message);
		ED_LOG(Shader, err, "Shader {}: failed to link shaders: {}", m_Id, message);
	}

	for (const auto& shaderId : m_ShadersIds) {
		glDetachShader(m_Id, shaderId);
		glDeleteShader(shaderId);
	}
}

uint32_t OpenGLShader::GetID() const
{
    return m_Id;
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_Id);
}
