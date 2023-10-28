#include "Shader.h"

#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& path)
{
	std::unordered_map<uint32_t, std::string> sources;

	std::string source;
	uint32_t currentShaderType = UINT32_MAX;

	std::fstream file(path, std::ios_base::in);
	std::string line;

	while (std::getline(file, line)) {
		if (line.find("// type") != std::string::npos) {
			if (currentShaderType != UINT32_MAX) {
				sources[currentShaderType] = source;
			} 
			source.clear();
			 
			if (line.find("fragment") != std::string::npos) {
				currentShaderType = GL_FRAGMENT_SHADER;
			} else if (line.find("vertex") != std::string::npos) {
				currentShaderType = GL_VERTEX_SHADER;
			} else if (line.find("geometry") != std::string::npos)
			{
				currentShaderType = GL_GEOMETRY_SHADER;
			} else {
				std::cout << "ERROR: NOT A SUPPORTED SHADER TYPE" << std::endl;
				currentShaderType = UINT32_MAX;
			}
		} else {
			source += line + "\n";
		}
	}

	if (currentShaderType != UINT32_MAX) {
		sources[currentShaderType] = source;
	}
	
	CreateProgram(sources);
}

Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
	CreateProgram({
		{ GL_VERTEX_SHADER, vertexShaderSource },
		{ GL_FRAGMENT_SHADER, fragmentShaderSource }
	});
}

void Shader::Bind() const
{
	glUseProgram(m_Id);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int32_t value) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform1i(location, value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform1f(location, value);
}

void Shader::SetFloat2(const std::string& name, float x, float y)
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform2f(location, x, y);
}

void Shader::SetFloat3(const std::string& name, float x, float y, float z) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform3f(location, x, y, z);
}

void Shader::SetFloat3(const std::string& name, glm::vec3 vector) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::SetFloat4(const std::string& name, float r, float g, float b, float a) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform4f(location, r, g, b, a);
}

void Shader::SetFloat4(const std::string& name, glm::vec4 vector) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& matrix) const
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetBool(const std::string& name, bool value)
{
	const int32_t location = glGetUniformLocation(m_Id, name.c_str());
	glUniform1i(location, value);
}

uint32_t Shader::CreateShader(uint32_t shaderType, const std::string& shaderSource) {
	const uint32_t shaderId = glCreateShader(shaderType);

	const char* shaderSourceC = shaderSource.data();
	glShaderSource(shaderId, 1, &shaderSourceC, 0);

	glCompileShader(shaderId);

	int32_t status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char message[1024];
		int32_t messageLength;
		glGetShaderInfoLog(shaderId, 1024, &messageLength, message);
		std::cout << shaderType << "::" << message;
	}

	return shaderId;
}

void Shader::CreateProgram(const std::unordered_map<uint32_t, std::string>& sources)
{
	m_Id = glCreateProgram();

	for (const auto& [type, source] : sources) {
		uint32_t shaderId = CreateShader(type, source);

		m_ShadersIds[type] = shaderId;

		glAttachShader(m_Id, shaderId);
	}

	glLinkProgram(m_Id);
	int32_t status;
	glGetProgramiv(m_Id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {	
		char message[1024];
		int32_t messageLength;
		glGetProgramInfoLog(m_Id, 1024, &messageLength, message);
		std::cout << message << std::endl;						
	}
			
	glValidateProgram(m_Id);

	for (const auto& [type, shaderId] : m_ShadersIds) {
		glDetachShader(m_Id, shaderId);
		glDeleteShader(shaderId);
	}
}
