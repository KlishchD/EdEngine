#include "Shader.h"

Shader::Shader(ShaderType type, const std::string& source) : m_Type(type), m_Source(source)
{

}

ShaderType Shader::GetType() const
{
	return m_Type;
}
