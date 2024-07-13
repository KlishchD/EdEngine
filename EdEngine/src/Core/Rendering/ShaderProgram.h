#pragma once

#include "Shader.h"

class ShaderProgram : public Resource
{
public:
  virtual void AttachShader(std::shared_ptr<Shader> shader) = 0;
  virtual void LinkProgram() = 0;
  virtual void DetachAllShaders() = 0;

  const std::vector<std::shared_ptr<Shader>>& GetAttachedShaders() const;
  const std::vector<std::shared_ptr<Shader>>& GetLinkedShaders() const;

  virtual ~ShaderProgram() = default;
protected:
  std::vector<std::shared_ptr<Shader>> m_AttachedShaders;
  std::vector<std::shared_ptr<Shader>> m_LinkedShaders;
};