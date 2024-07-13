#include "StringHelper.h"

std::wstring StringHelper::StringToWCHAR(const std::string& str)
{
  uint32_t sizeRequired = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), nullptr, 0);

  ED_ASSERT(sizeRequired >= 0, "WCHAR to char conversion failed");

  std::wstring result(sizeRequired, '*');

  MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), result.data(), sizeRequired);

  return result;
}


