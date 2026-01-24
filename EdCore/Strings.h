#pragma once

namespace Strings
{
    cstr8 RequestString(ccstr8 str, u32 size, bool temporary);
    cstr8 RequestString(u32 size, bool temporary, bool clear = true);

    cstr16 RequestWideString(ccstr16 str, u32 size, bool temporary);
    cstr16 RequestWideString(u32 size, bool temporary, bool clear = true);

    ccstr8 Convert(ccstr16 str, i32 size, bool temporary);
    ccstr16 Convert(ccstr8 str, i32 size, bool temporary);
    
    ccstr8 Convert(ccstr16 str, bool temporary);
    ccstr16 Convert(ccstr8 str, bool temporary);

    cstr8 Append(cstr8 destination, ccstr8 a1);
    cstr8 Concat(cstr8 destination, ccstr8 a1);
    cstr8 Concat(cstr8 destination, ccstr8 a1, ccstr8 a2);
    cstr8 Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3);
    cstr8 Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4);
    cstr8 Concat(cstr8 destination, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4, ccstr8 a5);

    cstr8 Concat(u32 size, ccstr8 a1, ccstr8 a2);
    cstr8 Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3);
    cstr8 Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4);
    cstr8 Concat(u32 size, ccstr8 a1, ccstr8 a2, ccstr8 a3, ccstr8 a4, ccstr8 a5);

    bool EqualCap(ccstr8 a, ccstr8 b, u32 cap);
    bool Equal(ccstr8 a, ccstr8 b);

    bool IsNumeric(ccstr8 string);

    template <typename T>
    T ParseNumeric(ccstr8 string)
    {
        static_assert(std::is_same_v<T, i32> || std::is_same_v<T, u32> || std::is_same_v<T, i64> || std::is_same_v<T, u64> || std::is_same_v<T, f64> || std::is_same_v<T, f32>, "Type is not supported.");

        if constexpr (std::is_same_v<T, i32> || std::is_same_v<T, u32>)
        {
            return atoi(string);
        }
        else if constexpr (std::is_same_v<T, i64> || std::is_same_v<T, u64>)
        {
            return atoll(string);
        }
        else if constexpr (std::is_same_v<T, f64> || std::is_same_v<T, f32>)
        {
            return atof(string);
        }
    }
};