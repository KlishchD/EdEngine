#pragma once

#include "EdEngine.h"

class PlatformHelper
{
public:
    static Path OpenFileWindow(const c8* filter, Window& window, const c8* title = nullptr, const c8* extension = nullptr);
    static Path SaveFileWindow(const c8* filter, Window& window, const c8* title = nullptr, const c8* extension = nullptr);
};
