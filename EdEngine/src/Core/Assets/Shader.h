#pragma once

#include "Core/Ed.h"

class Shader {
public:
	Shader(const std::string& path);
	Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

	void Bind() const;
	void Unbind() const;

	void SetInt(const std::string& name, int32_t value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetFloat2(const std::string& name, float x, float y);
	void SetFloat3(const std::string& name, float x, float y, float z) const;
	void SetFloat3(const std::string& name, glm::vec3 vector) const;
	void SetFloat4(const std::string& name, float r, float g, float b, float a) const;
	void SetFloat4(const std::string& name, glm::vec4 vector) const;
	void SetMat4(const std::string& name, const glm::mat4& matrix) const;
	void SetMat3(const std::string& name, const glm::mat3& matrix) const;
	void SetBool(const std::string& name, bool value);

	void SetInt(const char* name, int32_t value) const;
	void SetFloat(const char* name, float value) const;
	void SetFloat2(const char* name, float x, float y);
	void SetFloat3(const char* name, float x, float y, float z) const;
	void SetFloat3(const char* name, glm::vec3 vector) const;
	void SetFloat4(const char* name, float r, float g, float b, float a) const;
	void SetFloat4(const char* name, glm::vec4 vector) const;
	void SetMat4(const char* name, const glm::mat4& matrix) const;
	void SetMat3(const char* name, const glm::mat3& matrix) const;
	void SetBool(const char* name, bool value);

private: 
	uint32_t m_Id;
	std::unordered_map<uint32_t, uint32_t> m_ShadersIds;

	uint32_t CreateShader(uint32_t shaderType, const std::string& shaderCode);
	void CreateProgram(const std::unordered_map<uint32_t, std::string>& sources);
};
