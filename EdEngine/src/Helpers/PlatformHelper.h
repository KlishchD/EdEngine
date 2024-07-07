#pragma once

#include "Core/Rendering/EdRendering.h"

class Window;

class PlatformHelper
{
public:
    static std::string OpenFileWindow(const char* filter, Window& window, const char* title = nullptr);
    static std::string SaveFileWindow(const char* filter, Window& window, const char* title = nullptr);
    static void DisableTitleBar(Window& window);
};
