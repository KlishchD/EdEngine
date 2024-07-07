#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <system_error>

#include "Platform/Rendering/EdRendering.h"
#include <GLFW/glfw3native.h>

#include "D3D12Types.h"
#include "Helpers/D3D12Helper.h"

namespace D3D
{
	static void Check(HRESULT result)
	{
		ED_ASSERT(SUCCEEDED(result), "D3D11 error: {}", std::system_category().message(result));
	}
}

inline class D3D12RenderingContext* gContext = nullptr;