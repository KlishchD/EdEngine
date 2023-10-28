#pragma once

#include "Core/Ed.h"

class IndexBuffer
{
public:
    IndexBuffer(void* data, uint32_t size);

    void SetData(uint32_t* data, uint32_t size);
    void SetData(uint32_t* data);

    void Bind();
    void Unbind();

    uint32_t GetCount();

    ~IndexBuffer();
private:
    uint32_t m_Id;

    uint32_t m_Size;
};
