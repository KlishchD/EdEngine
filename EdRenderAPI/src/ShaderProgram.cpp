#include "EdRenderApiPrivate.h"
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : m_Dirty(true)
{

}

void ShaderProgram::AttachShader(Shader* shader)
{
    ED_ASSERT(shader, "Can not attach nullptr shader.");

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    for (Shader* attached : m_AttachedShaders)
    {
        ED_ASSERT(attached->GetType() != shader->GetType(), "Can not attach two shaders of the same type to the program.");
    }
#endif

    m_Dirty = true;

    m_AttachedShaders.Add(shader);
}

void ShaderProgram::AttachShader(const ContentPath& path, ShaderType type)
{
    Shader* shader = RenderingContext::Get().CreateShader(path, type);
    AttachShader(shader);
}

bool ShaderProgram::Compile(bool debug, cstr8 message)
{
    for (Shader* shader : m_AttachedShaders)
    {
        bool compiled = shader->Compile(m_Defines, debug, message);
        if (!compiled)
        {
            return false;
        }
    }

    m_Dirty = false;

    return true;
}

void ShaderProgram::AddDefine(ccstr8 define)
{
    m_Defines.Add(define);
    m_Dirty = true;
}

Shader* ShaderProgram::GetShader(ShaderType type)
{
    for (Shader* shader : m_AttachedShaders)
    {
        if (shader->GetType() == type)
        {
            return shader;
        }
    }

    return nullptr;
}

const Array<Shader*>& ShaderProgram::GetAttachedShaders() const
{
    return m_AttachedShaders;
}

bool ShaderProgram::HasShader(ShaderType types) const
{
    u32 mask = static_cast<u32>(types);

    for (Shader* shader : m_AttachedShaders)
    {
        mask &= ~static_cast<u32>(shader->GetType());
    }

    return mask == 0;
}
