#pragma once

template <typename T> requires(std::is_same_v<T, u8> || std::is_same_v<T, u16> || std::is_same_v<T, u32> || std::is_same_v<T, u64>)
class BitMask
{
    constexpr static inline u32 bits = sizeof(T) * 8;
public:
    BitMask(u32 count)
    {
        m_Size = (count + bits - 1) / bits;
        m_Mask = Memory::Get().RequestStaticMemory<T>(m_Size, 1, "BitMask");
        memset(m_Mask, 0, m_Size);
    }

    inline bool Get(u32 position) const
    {
        ED_ASSERT(position <= m_Size * bits, "Position lies outside of the BitMask range.");
        T* slot = m_Mask + (position / bits);
        T bit = 1 << (position % bits);
        return (*slot) & bit;
    }

    inline void Set(u32 position, bool state)
    {
        ED_ASSERT(position <= m_Size * bits, "Position lies outside of the BitMask range.");
        T* slot = (m_Mask + (position / bits));
        u32 bit = 1 << (position % bits);
        (*slot) = ((*slot) & ~bit) | bit;
    }

    void Clear()
    {
        memset(m_Mask, 0, m_Size * sizeof(T));
    }
protected:
    T* m_Mask;
    u32 m_Size;
};

using BitMask8 = BitMask<u8>;
using BitMask16 = BitMask<u16>;
using BitMask32 = BitMask<u32>;
using BitMask64 = BitMask<u64>;