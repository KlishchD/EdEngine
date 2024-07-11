#pragma once

#include "Core/Ed.h"

class UploadBuffer
{
public:
  template <typename T>
  static UploadBuffer Create(uint32_t count = 1)
  {
    return UploadBuffer(sizeof(T) * count);
  }
  
  UploadBuffer(uint32_t size = 1);
  UploadBuffer(const UploadBuffer& buffer);
  UploadBuffer(UploadBuffer&& buffer);
  UploadBuffer operator=(UploadBuffer& buffer);
  UploadBuffer operator=(UploadBuffer&& buffer);

  template <typename T>
  void Add(const T& object)
  {
    uint32_t objectSize = sizeof(T);

    while (m_FreeStart + objectSize > m_Memory + m_Size)
    {
      Expand();
    }

    memcpy(m_FreeStart, &object, objectSize);
    m_FreeStart += objectSize;
  }

  template <typename T, typename ..._Types>
  void Add(_Types&& ...args)
  {
    uint32_t objectSize = sizeof(T);

    while (m_FreeStart + objectSize > m_Memory + m_Size)
    {
      Expand();
    }

    new (m_FreeStart) T(std::forward<_Types>(args)...);

    m_FreeStart += objectSize;
  }

  void relinquishData();

  uint8_t* GetBegin() const;

  uint8_t* GetEnd() const;

  uint32_t GetOccupiedSize() const;

  uint32_t GetTotalSize() const;

  template<typename T>
  uint32_t GetCount() const
  {
    return (m_FreeStart - m_Memory) / sizeof(T);
  }
  
  virtual ~UploadBuffer();
protected:
  void Expand();
protected:
  uint8_t* m_Memory;
  uint8_t* m_FreeStart;
  uint32_t m_Size;

  bool m_IsOwner = true;
};
