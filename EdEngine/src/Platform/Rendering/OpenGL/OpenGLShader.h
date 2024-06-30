#pragma once

#include "Core/Rendering/Shader.h"
#include <vector>

class OpenGLShader : public Shader {
public:
	OpenGLShader(ShaderType type, const std::string& filepath, const std::string& source);

	uint32_t GetID() const;

	virtual ~OpenGLShader() override;
private:
	uint32_t m_Id;
};