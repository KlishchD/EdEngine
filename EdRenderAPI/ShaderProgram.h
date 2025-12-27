#pragma once

#include "Shader.h"

class ShaderProgram
{
public:
    ShaderProgram();

    void AttachShader(Shader* shader);
    void AttachShader(const ContentPath& path, ShaderType type);
    
    bool Compile(bool debug, cstr8 message);
    void AddDefine(ccstr8 define);

    bool NeedsRecompilation() const { return m_Dirty; }

    Shader* GetShader(ShaderType type);
    const Array<Shader*>& GetAttachedShaders() const;

    bool HasShader(ShaderType types) const;
protected:
    bool m_Dirty;

    Array<Shader*> m_AttachedShaders;
    Array<Shader*> m_LinkedShaders;

    Array<ccstr8> m_Defines;
};
