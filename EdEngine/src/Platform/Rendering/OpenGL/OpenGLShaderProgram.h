#pragma once

#include "Core/Rendering/ShaderProgram.h"

class OpenGLShaderProgram : public ShaderProgram
{
public:
	OpenGLShaderProgram();

	virtual void AttachShader(std::shared_ptr<Shader> inShader) override;
	virtual void LinkProgram() override;
	virtual void DetachAllShaders() override;

	uint32_t GetID() const;

	virtual ~OpenGLShaderProgram() override;
protected:
	uint32_t m_Id;
};
