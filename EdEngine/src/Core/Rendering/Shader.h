#pragma once

#include "Core/Rendering/EdRendering.h"

class Shader : public Resource {
public:
	Shader(ShaderType type, const std::string& source);
	virtual ~Shader() = default;

	ShaderType GetType() const;
protected:
	ShaderType m_Type;

	// TODO: Make it only exist with editor
	std::string m_Source;
};
