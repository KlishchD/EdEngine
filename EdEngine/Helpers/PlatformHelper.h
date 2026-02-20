#pragma once

#include "EdEngine.h"

class PlatformHelper
{
public:
    static Path OpenFileWindow(const char* filter, Window& window, const char* base_path = nullptr, const char* title = nullptr, const char* extension = nullptr);
    static Path SaveFileWindow(const char* filter, Window& window, const char* base_path = nullptr, const char* title = nullptr, const char* extension = nullptr);
};
