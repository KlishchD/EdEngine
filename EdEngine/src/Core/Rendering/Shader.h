#pragma once

#include "Types.h"
#include "Core/Ed.h"

class Shader {
public:
	Shader(ShaderType type, const std::string& source);
	virtual ~Shader() = default;

	ShaderType GetType() const;
protected:
	ShaderType m_Type;

	// TODO: Make it only exist with editor
	std::string m_Source;
};
