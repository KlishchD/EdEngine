﻿#pragma once

#include "Core/Window.h"

class PlatformUtils
{
public:
    static std::string OpenFileWindow(const char* filter, Window& window, const char* title = nullptr);
    static std::string SaveFileWindow(const char* filter, Window& window, const char* title = nullptr);
};
