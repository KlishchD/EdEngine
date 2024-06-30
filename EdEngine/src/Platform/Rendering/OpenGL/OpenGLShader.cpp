#include "OpenGLShader.h"
#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"

//#include "DirStackFileIncluder.h"
//#include <glslang/Public/ShaderLang.h>
//#include <glslang/Public/ResourceLimits.h>
//#include <glslang/SPIRV/GlslangToSpv.h>
//#include <glslang/SPIRV/disassemble.h>
//#include <spirv-tools/optimizer.hpp>
//#include <spirv_cross/spirv_glsl.hpp>

OpenGLShader::OpenGLShader(ShaderType type, const std::string& filepath, const std::string& source) : Shader(type, source)
{
	uint32_t shaderType = OpenGLTypes::ConvertShaderType(type);
	m_Id = glCreateShader(shaderType);
// 
// 	EShLanguage stage = static_cast<EShLanguage>(OpenGLTypes::ConvertShaderLanguage(type));
// 	glslang::TShader shader(static_cast<EShLanguage>(stage));
// 
 	const char* shaderSourceC = &source[0];
// 	shader.setStrings(&shaderSourceC, 1);
// 
// 	const std::string entryPoint = Types::ConvertShaderEntryPointName(type);
// 	shader.setEntryPoint(entryPoint.c_str());
// 
// 	shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientOpenGL, 460);
// 	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);
// 
// 	shader.setAutoMapLocations(true);
// 
// 	DirStackFileIncluder includer(filepath);
// 	bool isCompilationSuccessfull = shader.parse(GetDefaultResources(), 110, ECoreProfile, false, false, EShMsgDefault, includer);
// 	ED_ASSERT(isCompilationSuccessfull, "Shader GLSL to Spir-V compilation failed {}, {}", filepath, shader.getInfoLog());
// 
// 	spv::SpvBuildLogger logger;
// 
// 	glslang::SpvOptions spvOptions;
// 	spvOptions.generateDebugInfo = true;
// 
// 	std::vector<uint32_t> unoptimisedSpv;
// 	glslang::GlslangToSpv(*shader.getIntermediate(), unoptimisedSpv, &logger, &spvOptions);
// 
// 	glShaderBinary(1, &m_Id, GL_SHADER_BINARY_FORMAT_SPIR_V, unoptimisedSpv.data(), sizeof(uint32_t) * unoptimisedSpv.size());
// 	glSpecializeShader(m_Id, "main", 0, nullptr, nullptr);

	glShaderSource(m_Id, 1, &shaderSourceC, 0);
	glCompileShader(m_Id);

	int32_t status = 0;
	glGetShaderiv(m_Id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		int32_t length = 0;
		glGetShaderiv(m_Id, GL_INFO_LOG_LENGTH, &length);

		std::string message(length, '*');
		glGetShaderInfoLog(m_Id, length, &length, &message[0]);

		ED_ASSERT(0, "Shader compilation error: {}", message);
	}
}

uint32_t OpenGLShader::GetID() const
{
    return m_Id;
}

OpenGLShader::~OpenGLShader()
{
	glDeleteShader(m_Id);
}
