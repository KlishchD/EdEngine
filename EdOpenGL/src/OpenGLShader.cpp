#include "EdOpenGL.h"
#include "Shader.h"

Shader::Shader(const std::vector<std::pair<ShaderType, std::string>>& shaders)
{
	m_NativeHandle = glCreateProgram();

	i32 ids[6] = { -1, -1, -1, -1, -1, -1 };

	for (const auto& [type, source] : shaders)
	{
		const u32 shaderId = glCreateShader(OpenGLTypes::ConvertShaderType(type));

		const c8* shaderSourceC = source.data();
		glShaderSource(shaderId, 1, &shaderSourceC, 0);

		glCompileShader(shaderId);

		i32 status;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) 
		{
			c8 message[1024];
			i32 messageLength;
			glGetShaderInfoLog(shaderId, 1024, &messageLength, message);
			ED_LOG(Shader, err, "Shader {}: failed to compile shader: {}", m_NativeHandle, message);
		}

		glAttachShader(m_NativeHandle, shaderId);

		ids[static_cast<i32>(type)] = shaderId;
	}

    glLinkProgram(m_NativeHandle);

    glValidateProgram(m_NativeHandle);

    i32 status;
    glGetProgramiv(m_NativeHandle, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        c8 message[1024];
        i32 messageLength;
        glGetProgramInfoLog(m_NativeHandle, 1024, &messageLength, message);
        ED_LOG(Shader, err, "Shader {}: failed to link shaders: {}", m_NativeHandle, message);
    }

	for (i32 i = 0; i < std::size(ids); ++i)
	{
		const i32 shaderId = ids[i];
		if (shaderId != -1)
		{
			glDetachShader(m_NativeHandle, shaderId);
			glDeleteShader(shaderId);
		}
	}
}

Shader::~Shader()
{
	glDeleteProgram(m_NativeHandle);
}
