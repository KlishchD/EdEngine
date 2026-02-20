#include "EdEngine.h"
#include "Helpers/PlatformHelper.h"
#include <Windows.h>

Path PlatformHelper::OpenFileWindow(const char* filter, Window& window, const char* base_path, const char* title, const char* extension)
{
  OPENFILENAMEA descriptor;
  ZeroMemory(&descriptor, sizeof(OPENFILENAMEA));

  descriptor.lStructSize = sizeof(descriptor);
  descriptor.hwndOwner = (HWND)window.GetPlatformNativeWindow();
  descriptor.hInstance = 0;

  char filter_safe[512]{};
  std::strcpy(filter_safe, filter);
  descriptor.lpstrFilter = filter_safe;

  descriptor.lpstrCustomFilter = nullptr;
  descriptor.nMaxCustFilter = 0;

  descriptor.nFilterIndex = 1;

  char buffer[256]{};
  descriptor.lpstrFile = buffer;
  descriptor.nMaxFile = 256;

  descriptor.lpstrFileTitle = nullptr;
  descriptor.nMaxFileTitle = 0;

  descriptor.lpstrInitialDir = base_path;
  descriptor.lpstrTitle = title;

  descriptor.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
  descriptor.nFileOffset = 0;
  descriptor.nFileExtension = 0;
  descriptor.lpstrDefExt = extension;
  descriptor.lCustData = 0;
  descriptor.lpfnHook = 0;
  descriptor.lpTemplateName = 0;

  return GetOpenFileNameA(&descriptor) == TRUE ? buffer : "";
}

Path PlatformHelper::SaveFileWindow(const char* filter, Window& window, const char* base_path, const char* title, const char* extension)
{
  OPENFILENAMEA descriptor;
  ZeroMemory(&descriptor, sizeof(OPENFILENAMEA));

  descriptor.lStructSize = sizeof(descriptor);
  descriptor.hwndOwner = (HWND)window.GetPlatformNativeWindow();
  descriptor.hInstance = 0;

  char filter_safe[512]{};
  std::strcpy(filter_safe, filter);
  descriptor.lpstrFilter = filter_safe;

  descriptor.lpstrCustomFilter = nullptr;
  descriptor.nMaxCustFilter = 0;

  descriptor.nFilterIndex = 1;

  char buffer[256]{};
  descriptor.lpstrFile = buffer;
  descriptor.nMaxFile = 256;

  descriptor.lpstrFileTitle = nullptr;
  descriptor.nMaxFileTitle = 0;

  descriptor.lpstrInitialDir = base_path;
  descriptor.lpstrTitle = title;

  descriptor.Flags = OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
  descriptor.nFileOffset = 0;
  descriptor.nFileExtension = 0;
  descriptor.lpstrDefExt = extension;
  descriptor.lCustData = 0;
  descriptor.lpfnHook = 0;
  descriptor.lpTemplateName = 0;

  return GetSaveFileNameA(&descriptor) == TRUE ? buffer : "";
}