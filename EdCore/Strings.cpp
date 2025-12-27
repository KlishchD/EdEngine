#include "EdCore.h"
#include "Strings.h"
#include <Windows.h>

cstr8 Strings::RequestString(ccstr8 str, u32 size, bool temporary)
{
    cstr8 result = Memory::Get().RequestMemory<c8>(str, strnlen(str, size), size + 1, temporary, false);
    result[size] = 0;
    return result;
}

cstr8 Strings::RequestString(u32 size, bool temporary, bool clear)
{
    cstr8 result = Memory::Get().RequestMemory<c8>(size + 1, temporary, clear);
    result[size] = 0;
    return result;
}

cstr16 Strings::RequestWideString(ccstr16 str, u32 size, bool temporary)
{
    cstr16 result = Memory::Get().RequestMemory<c16>(str, wcsnlen(str, size), size + 1, temporary, false);
    result[size] = 0;
    return result;
}

cstr16 Strings::RequestWideString(u32 size, bool temporary, bool clear)
{
    cstr16 result = Memory::Get().RequestMemory<c16>(size + 1, temporary, clear);
    result[size] = 0;
    return result;
}

ccstr16 Strings::Convert(ccstr8 str, i32 size, bool temporary)
{
    if (size <= 0)
    {
        size = strnlen(str, 1024);
    }

    uint32_t sizeRequired = MultiByteToWideChar(CP_UTF8, 0, str, size, nullptr, 0);
    ED_ASSERT(sizeRequired >= 0, "WCHAR to c8 conversion failed");

    ccstr16 result = RequestWideString(sizeRequired, temporary, false);
    MultiByteToWideChar(CP_UTF8, 0, str, size, const_cast<c16*>(result), sizeRequired);

    return result;
}

ccstr8 Strings::Convert(ccstr16 str, i32 size, bool temporary)
{
    if (size <= 0)
    {
        size = wcsnlen(str, 1024);
    }

    uint32_t sizeRequired = WideCharToMultiByte(CP_UTF8, 0, str, size, nullptr, 0, nullptr, nullptr);
    ED_ASSERT(sizeRequired >= 0, "WCHAR to c8 conversion failed");

    ccstr8 result = RequestString(sizeRequired, temporary, false);
    WideCharToMultiByte(CP_UTF8, 0, str, size, const_cast<c8*>(result), sizeRequired, nullptr, nullptr);

    return result;
}

ccstr16 Strings::Convert(ccstr8 str, bool temporary)
{
    return Convert(str, 0, temporary);
}

ccstr8 Strings::Convert(ccstr16 str, bool temporary)
{
    return Convert(str, 0, temporary);
}

cstr8 Strings::Concat(cstr8 destination, ccstr8 a1)
{
    strcat(destination, a1);
    return destination;
}

bool Strings::EqualCap(ccstr8 a, ccstr8 b, u32 cap)
{
    return 0 == strncmp(a, b, cap);
}

bool Strings::Equal(ccstr8 a, ccstr8 b)
{
    return 0 == strcmp(a, b);
}

bool Strings::IsNumeric(ccstr8 string)
{
    while (string != '\0' && string[0] >= '0' && string[0] <= '9') ++string;
    return string == '\0';
}

cstr8 Strings::Concat(cstr8 destination, ccstr8 a1, ccstr8 a2)
{
    strcat(destination, a1);
    strcat(destination, a2);
    return destination;
}

cstr8 Strings::Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3)
{
    strcat(destination, a1);
    strcat(destination, a2);
    strcat(destination, a3);
    return destination;
}

cstr8 Strings::Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4)
{
    strcat(destination, a1);
    strcat(destination, a2);
    strcat(destination, a3);
    strcat(destination, a4);
    return destination;
}

cstr8 Strings::Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4, ccstr8 a5)
{
    strcat(destination, a1);
    strcat(destination, a2);
    strcat(destination, a3);
    strcat(destination, a4);
    strcat(destination, a5);
    return destination;
}

cstr8 Strings::Concat(u32 size, ccstr8 a1, ccstr8 a2)
{
    return Concat(RequestString(size, false, true), a1, a2);
}

cstr8 Strings::Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3)
{
    return Concat(RequestString(size, false, true), a1, a2, a3);
}

cstr8 Strings::Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4)
{
    return Concat(RequestString(size, false, true), a1, a2, a3, a4);
}

cstr8 Strings::Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4, ccstr8 a5)
{
    return Concat(RequestString(size, false, true), a1, a2, a3, a4, a5);
}