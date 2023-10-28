#pragma once

#include <vector>
#include <memory>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class VertexArray
{
public:
    VertexArray();
    VertexArray(const std::vector<std::shared_ptr<VertexBuffer>>& buffers);
    
    void AddBuffer(void* data, uint32_t size, const VertexBufferLayout& layout, uint32_t location);
    void AddBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t location);

    void SetIndexBuffer(void* data, uint32_t size);
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
    std::shared_ptr<IndexBuffer> GetIndexBuffer() const;
    
    void Bind() const;
    void Unbind() const;

    uint32_t GetCount() const;
    std::shared_ptr<VertexBuffer> GetBuffer(uint32_t index) const;

    ~VertexArray();
private:
    uint32_t m_Id;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};