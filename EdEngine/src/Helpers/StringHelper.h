#pragma once

#include <Core/Ed.h>
#include <Windows.h>

class StringHelper
{
public:
  template<uint32_t size>
  static std::string WCHARToString(WCHAR str[size]);

  static std::wstring StringToWCHAR(const std::string& str);
};


template<uint32_t size>
std::string StringHelper::WCHARToString(WCHAR str[size])
{
  uint32_t sizeRequired = WideCharToMultiByte(CP_UTF8, 0, str, size, nullptr, 0, nullptr, nullptr);

  ED_ASSERT(sizeRequired >= 0, "WCHAR to char conversion failed");

  std::string result(sizeRequired, '*');

  WideCharToMultiByte(CP_UTF8, 0, str, size, result.data(), sizeRequired, nullptr, nullptr);

  return result;
}
