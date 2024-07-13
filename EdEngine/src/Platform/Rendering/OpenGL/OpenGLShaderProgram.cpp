#include "OpenGLShaderProgram.h"
#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"
#include "OpenGLShader.h"

OpenGLShaderProgram::OpenGLShaderProgram()
{
  m_Id = glCreateProgram();
  glObjectLabel(GL_PROGRAM, m_Id, m_Name.size(), m_Name.c_str());
}

void OpenGLShaderProgram::AttachShader(std::shared_ptr<Shader> shader)
{
  glAttachShader(m_Id, shader->GetNativeResource<uint32_t>());

  m_AttachedShaders.push_back(shader);
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
  for (const std::shared_ptr<Shader>& shader : m_AttachedShaders)
  {
    glDetachShader(m_Id, shader->GetNativeResource<uint32_t>());
  }

  m_AttachedShaders.clear();
}

void* OpenGLShaderProgram::GetNativeResource() const
{
  return reinterpret_cast<void*>(m_Id);
}

void OpenGLShaderProgram::SetNativeResource(void* resource)
{
  m_Id = reinterpret_cast<uint32_t>(resource);
  glObjectLabel(GL_PROGRAM, m_Id, m_Name.size(), m_Name.c_str());
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
  glDeleteProgram(m_Id);
}
