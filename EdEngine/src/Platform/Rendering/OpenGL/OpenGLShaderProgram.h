#pragma once

#include "Core/Rendering/ShaderProgram.h"

class OpenGLShaderProgram : public ShaderProgram
{
public:
  OpenGLShaderProgram();

  virtual void AttachShader(std::shared_ptr<Shader> shader) override;
  virtual void LinkProgram() override;
  virtual void DetachAllShaders() override;

  virtual void* GetNativeResource() const override;
  virtual void SetNativeResource(void* resource) override;

  virtual ~OpenGLShaderProgram() override;

protected:
  uint32_t m_Id;
};
