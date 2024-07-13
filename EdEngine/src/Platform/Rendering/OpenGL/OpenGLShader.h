#pragma once

#include "Core/Rendering/Shader.h"

class OpenGLShader : public Shader
{
public:
  OpenGLShader(ShaderType type, const std::string& filepath, const std::string& source);

  virtual void* GetNativeResource() const override;
  virtual void SetNativeResource(void* resource) override;

  virtual ~OpenGLShader() override;

private:
  uint32_t m_Id;
};
