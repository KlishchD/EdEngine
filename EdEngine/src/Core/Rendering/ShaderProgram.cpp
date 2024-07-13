#include "ShaderProgram.h"

const std::vector<std::shared_ptr<Shader>>& ShaderProgram::GetAttachedShaders() const
{
  return m_AttachedShaders;
}

const std::vector<std::shared_ptr<Shader>>& ShaderProgram::GetLinkedShaders() const
{
  return m_LinkedShaders;
}
