#pragma once

#pragma comment(lib, "EdCore.lib")
#pragma comment(lib, "EdRenderAPI.lib")

#define DirectX12API 2
#define RenderAPI DirectX12API

#if RenderAPI == DirectX12API
    #pragma comment(lib, "EdDirectX12.lib")
#else
    #error Suplied render API is not supported.
#endif

#pragma comment(lib, "EdRenderer.lib")
#pragma comment(lib, "EdEngine.lib")

#pragma comment(lib, "ImGui.lib")

#include "EdEngine.h"

#include "imgui.h"

#include "Engine.h"
#include "Editor.h"

namespace ImGui
{
    inline void LoadIniSettingsFromDisk(const ContentPath& path)
    {
        LoadIniSettingsFromDisk(path.Get());
    }

    inline void SaveIniSettingsToDisk(const ContentPath& path)
    {
        SaveIniSettingsToDisk(path.Get());
    }
}