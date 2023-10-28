#include "Utils/PlatformUtils.h"
#include <windows.h>
#include <string>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

std::string PlatformUtils::OpenFileWindow(const char* filter, Window& window, const char* title)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(window.GetWindow());
    
    char filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrTitle = title;

    return GetOpenFileNameA(&ofn) == TRUE ? filepath : "";
}

std::string PlatformUtils::SaveFileWindow(const char* filter, Window& window, const char* title)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(window.GetWindow());
    
    char filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrTitle = title;

    return GetSaveFileNameA(&ofn) == TRUE ? filepath : "";
}
