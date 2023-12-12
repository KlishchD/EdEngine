#pragma once

#include "Types.h"
#include <string>

class Shader {
public:
	virtual void SetShaderCode(ShaderType type, const std::string& code) = 0;
	virtual ~Shader() = default;
};
