#pragma once

#define D3D_DEBUG 0
#define D3D_FORCE_EVENTS_OFF 0

#if D3D_DEBUG
    #pragma comment(lib, "dxguid.lib")
#endif

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#ifndef NOMINMAX
    #define NOMINMAX  // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#if D3D_DEBUG
    #include "dxgidebug.h"
    #include "d3d12sdklayers.h"
#endif

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <system_error>

#include "EdRenderApiPrivate.h"

#include "DirectX12Types.h"

#if RELEASE_BUILD
    #undef D3D_DEBUG
    #define D3D_DEBUG 0
#endif

template <typename T>
static void SetObjectDebugName(T* object, ccstr8 name, i32 size)
{
    if (size == -1)
    {
        size = strnlen(name, 1024);
    }

    object->SetName(Strings::Convert(name, true));
}

template <typename T>
static void GetObjectDebugName(T* object, ccstr8& name, i32& size)
{
    ED_ASSERT(0, "Method is not supported by the API.");
    name = "Not supported by API :(";
    size = strlen(name);
}

#define NAME_METHODS_TEMPLATE(clazz, apiClazz) \
    void clazz::SetDebugName(ccstr8 name, i32 size) { SetObjectDebugName(GetNativeHandle<apiClazz>(), name, size); } \
    void clazz::GetDebugName(ccstr8& name, i32& size) { GetObjectDebugName(GetNativeHandle<apiClazz>(), name, size); }

//#include "Helpers/D3D12Helper.h"

inline Microsoft::WRL::ComPtr<IDXGIFactory6> g_Factory;
inline Microsoft::WRL::ComPtr<IDXGIAdapter4> g_Adapter;
inline Microsoft::WRL::ComPtr<ID3D12Device1> g_Device;

namespace D3D
{
    static void Check(HRESULT result)
    {
        ED_ASSERT(SUCCEEDED(result), "D3D12 error: {}", std::system_category().message(result));
    }
}   