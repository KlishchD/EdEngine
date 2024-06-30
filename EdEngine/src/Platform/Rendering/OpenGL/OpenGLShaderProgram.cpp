#include "OpenGLShaderProgram.h"
#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"
#include "OpenGLShader.h"

OpenGLShaderProgram::OpenGLShaderProgram()
{
	m_Id = glCreateProgram();
}

void OpenGLShaderProgram::AttachShader(std::shared_ptr<Shader> inShader)
{
	std::shared_ptr<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(inShader);
	glAttachShader(m_Id, shader->GetID());

	m_AttachedShaders.push_back(inShader);
}

void OpenGLShaderProgram::LinkProgram()
{
	glLinkProgram(m_Id);

	glValidateProgram(m_Id);

	int32_t status = 0;
	glGetProgramiv(m_Id, GL_LINK_STATUS, &status);
	
	if (status == GL_FALSE)
	{
		int32_t length;
		glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &length);

		std::string message(length, '*');
		glGetProgramInfoLog(m_Id, length, &length, &message[0]);

		ED_LOG(Shader, err, "Shader program failed to link: {}", message);
	}
	else
	{
		m_LinkedShaders = m_AttachedShaders;
	}
}

void OpenGLShaderProgram::DetachAllShaders()
{
	for (const std::shared_ptr<Shader>& attachedShader : m_AttachedShaders)
	{
		std::shared_ptr<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(attachedShader);
		glDetachShader(m_Id, shader->GetID());
	}

	m_AttachedShaders.clear();
}

uint32_t OpenGLShaderProgram::GetID() const
{
	return m_Id;
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
	glDeleteProgram(m_Id);
}
