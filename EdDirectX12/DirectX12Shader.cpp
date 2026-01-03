#include "EdDirectX12.h"
#include "Helpers/FilesHelper.h"

#define GetBlob() reinterpret_cast<ID3DBlob*>(m_NativeHandle)

struct IncludeHandler : public IDxcIncludeHandler
{
    struct ShaderInstance
    {

    };

    virtual HRESULT STDMETHODCALLTYPE LoadSource(_In_z_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
    {
        //ccstr8 filename = Strings::Convert(pFilename, true);
        //
        //ContentPath path = Files::ShadersRootDirectory;
        //path.ReplaceFile(filename);
        


        return 0;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override { return 0; }
    virtual ULONG STDMETHODCALLTYPE AddRef() override { return 0; }
    virtual ULONG STDMETHODCALLTYPE Release(void) override { return 0; }
};

Shader::Shader(ShaderType type, const ContentPath& filepath) : m_NativeHandle(0), m_Type(type), m_Path(filepath)
{

}

bool Shader::Compile(const Array<ccstr8>& defines, bool debug, cstr8 message)
{
    ccstr16 path = Strings::Convert(m_Path.Get(), true);
    ccstr16 entrypoint = RenderTypes::ConvertShaderEntrypoint(m_Type);
    ccstr16 target = RenderTypes::ConvertShaderTarget(m_Type);
    ccstr16 rootDirectory = Strings::Convert(Files::GetShadersPath().Get(), true);

    static Microsoft::WRL::ComPtr<IDxcUtils> s_Utils;
    static Microsoft::WRL::ComPtr<IDxcCompiler3> s_Compiler;
    static Microsoft::WRL::ComPtr<IDxcIncludeHandler> s_IncludeHandler;
    //static IncludeHandler s_IncludeHandler;

    if (!s_Utils)
    {
        D3D::Check(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&s_Utils)));
        D3D::Check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s_Compiler)));
        D3D::Check(s_Utils->CreateDefaultIncludeHandler(&s_IncludeHandler));
    }

    Microsoft::WRL::ComPtr<IDxcBlobEncoding> blob;
    s_Utils->LoadFile(path, nullptr, &blob);

    DxcBuffer buffer;
    buffer.Ptr = blob->GetBufferPointer();
    buffer.Size = blob->GetBufferSize();
    buffer.Encoding = 0;

    // TODO: It is not the best practice to make dependent on these flags, it is better to make a console command
    // that can be switched in runtime or to have separate flags, because it in the future it could make 
    // the hella long wait for non render developers that do not even need this.
    // And better yet it is better to move to PSO's.
    TemporaryArray<ccstr16> arguments;
    arguments.Add(L"-E");
    arguments.Add(entrypoint);
    arguments.Add(L"-T");
    arguments.Add(target);
    arguments.Add(L"-I");
    arguments.Add(rootDirectory);
    arguments.Add(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);

    if (debug)
    {
        arguments.Add(DXC_ARG_WARNINGS_ARE_ERRORS);
        arguments.Add(DXC_ARG_ALL_RESOURCES_BOUND);
        arguments.Add(DXC_ARG_SKIP_OPTIMIZATIONS);
        arguments.Add(DXC_ARG_DEBUG);
        arguments.Add(DXC_ARG_DEBUG_NAME_FOR_SOURCE);
        arguments.Add(L"-Qembed_debug");
    }
    else
    {
        arguments.Add(DXC_ARG_OPTIMIZATION_LEVEL3);
    }

    for (ccstr8 define : defines)
    {
        arguments.Add(L"-D");
        arguments.Add(Strings::Convert(define, false));
    }

    Microsoft::WRL::ComPtr<IDxcResult> result;
    D3D::Check(s_Compiler->Compile(&buffer, arguments.Get(), arguments.GetSize(), s_IncludeHandler.Get(), IID_PPV_ARGS(&result)));

    Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;
    D3D::Check(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));

    if (errors && errors->GetStringLength())
    {
        std::string log(errors->GetStringPointer(), errors->GetStringLength());

        ED_LOG(Shader, err, "{}", log.c_str());
        ED_ASSERT(m_NativeHandle, "Was not able to compile shader and there is no substitutions for it.");
        
        if (message)
        {
            memcpy(message, log.c_str(), log.size());
        }

        return false;
    }
    
    if (m_NativeHandle)
    {
        GetBlob()->Release();
        m_NativeHandle = 0;
    }

    D3D::Check(result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), reinterpret_cast<void**>(&m_NativeHandle), nullptr));

    ED_LOG(Shader, info, "Compiled shader [{}]", m_Path.Get());

    return true;
}

bool Shader::IsCompiled() const
{
    return m_NativeHandle != 0;
}

u32 Shader::GetBytecodeLength() const
{
    return GetBlob()->GetBufferSize();
}

const void* Shader::GetBytecode() const
{
    return GetBlob()->GetBufferPointer();
}

Shader::~Shader()
{
    GetNativeHandle<ID3DBlob>()->Release();
}
