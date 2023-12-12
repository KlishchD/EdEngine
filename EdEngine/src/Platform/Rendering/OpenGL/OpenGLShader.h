#pragma once

#include "Core/Rendering/Shader.h"
#include <vector>

class OpenGLShader : public Shader {
public:
	OpenGLShader();

	virtual void SetShaderCode(ShaderType type, const std::string& code) override;
	void LinkProgram();

	uint32_t GetID() const;

	virtual ~OpenGLShader() override;
private:
	uint32_t m_Id;
	std::vector<uint32_t> m_ShadersIds;
};