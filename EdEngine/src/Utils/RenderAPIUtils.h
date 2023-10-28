#pragma once

#include "Core/Ed.h"

enum class BlendFactor
{
    One,
    SourceAlpha,
    OneMinusSourceAlpha
};

enum class ShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
};

class RenderAPIUtils
{
public:
    static uint32_t GetShaderDataTypeOpenGLComponentCount(ShaderDataType shaderDataType) {
        switch (shaderDataType)
        {
        case ShaderDataType::Float:     return 1;
        case ShaderDataType::Float2:    return 2;
        case ShaderDataType::Float3:    return 3;
        case ShaderDataType::Float4:    return 4;
        }
        return 0;
    }
    
    static uint32_t GetShaderDataTypeOpenGLSize(ShaderDataType shaderDataType) {
        switch (shaderDataType)
        {
        case ShaderDataType::Float:     return 4;
        case ShaderDataType::Float2:    return 8;
        case ShaderDataType::Float3:    return 12;
        case ShaderDataType::Float4:    return 16;
        }
        return 0;
    }

    static uint32_t GetShaderDataTypeOpenGLType(ShaderDataType shaderDataType) {
        switch (shaderDataType)
        {
        case ShaderDataType::Float:     return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT;
        case ShaderDataType::Float3:    return GL_FLOAT;
        case ShaderDataType::Float4:    return GL_FLOAT;
        }
        return 0;
    }

    static uint32_t GetBlendFactorOpenGL(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::One: return GL_ONE;
        case BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        }
        return 0;
    }
};
