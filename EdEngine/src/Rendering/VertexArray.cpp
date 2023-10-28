#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_Id);
	glBindVertexArray(m_Id);
}

VertexArray::VertexArray(const std::vector<std::shared_ptr<VertexBuffer>>& buffers): VertexArray()
{
	for (int32_t i = 0; i < buffers.size(); ++i)
	{
		AddBuffer(buffers[i], i);
	}
}

void VertexArray::AddBuffer(void* data, uint32_t size, const VertexBufferLayout& layout, uint32_t location)
{
	AddBuffer(std::make_shared<VertexBuffer>(data, size, layout), location);
}

void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t location)
{
	glBindVertexArray(m_Id);
	
	vertexBuffer->Bind();
	m_VertexBuffers.push_back(vertexBuffer);

	for (const auto& layout : vertexBuffer->GetLayout().GetElements()) {
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location++, layout.Count, layout.Type, layout.Normalized, vertexBuffer->GetStride(), (void*)layout.Offset);
	}

	glBindVertexArray(0);
}

void VertexArray::SetIndexBuffer(void* data, uint32_t size)
{
	if (!m_IndexBuffer)
	{
		SetIndexBuffer(std::make_shared<IndexBuffer>(data, size));
	}
	else
	{
		m_IndexBuffer->SetData(static_cast<uint32_t*>(data), size);
	}
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	m_IndexBuffer = indexBuffer;
	
	glBindVertexArray(m_Id);
	m_IndexBuffer->Bind();
	glBindVertexArray(0);
}

std::shared_ptr<IndexBuffer> VertexArray::GetIndexBuffer() const
{
	return m_IndexBuffer;
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_Id);

	if (m_IndexBuffer)
	{
		m_IndexBuffer->Bind();
	}
}

void VertexArray::Unbind() const
{

	if (m_IndexBuffer)
	{
		m_IndexBuffer->Unbind();
	}
	
	glBindVertexArray(0);
}

uint32_t VertexArray::GetCount() const
{
	return m_IndexBuffer->GetCount();
}

std::shared_ptr<VertexBuffer> VertexArray::GetBuffer(uint32_t index) const
{
	return m_VertexBuffers[index];
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_Id);
}
