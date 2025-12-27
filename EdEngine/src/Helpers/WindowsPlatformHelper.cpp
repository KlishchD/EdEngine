#include "EdEngine.h"
#include "Helpers/PlatformHelper.h"
#include <Windows.h>

Path PlatformHelper::OpenFileWindow(const c8* filter, Window& window, const c8* title, const c8* extension)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = (HWND)window.GetPlatformNativeWindow();
    
    c8 filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrDefExt = extension;

    ofn.lpstrTitle = title;

    return GetOpenFileNameA(&ofn) == TRUE ? filepath : "";
}

Path PlatformHelper::SaveFileWindow(const c8* filter, Window& window, const c8* title, const c8* extension)
{
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));

    ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = (HWND)window.GetPlatformNativeWindow();

    c8 filepath[512];
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';    
    ofn.nMaxFile = sizeof(filepath);

    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = filter;

    ofn.lpstrDefExt = extension;

    ofn.lpstrTitle = title;

    return GetSaveFileNameA(&ofn) == TRUE ? filepath : "";
}
