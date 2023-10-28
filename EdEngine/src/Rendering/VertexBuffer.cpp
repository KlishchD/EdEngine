#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(void* data, int32_t size, const VertexBufferLayout& layout): m_Size(size)
{
	glGenBuffers(1, &m_Id);
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	SetLayout(layout);
}
VertexBuffer::VertexBuffer(void* data, int32_t size): m_Size(size)
{
	glGenBuffers(1, &m_Id);
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
	
void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_Layout = layout;
}

void VertexBuffer::SetData(void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ARRAY_BUFFER, m_Size, data, GL_STATIC_DRAW);
}

void VertexBuffer::SetData(void* data, int32_t size)
{
	m_Size = size;
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferData(GL_ARRAY_BUFFER, m_Size, data, GL_STATIC_DRAW);
}

const VertexBufferLayout& VertexBuffer::GetLayout() const
{
	return m_Layout;
}

uint32_t VertexBuffer::GetStride() const
{
	return m_Layout.GetStride();
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetSubData(uint32_t offset, uint32_t size, void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_Id);
}

std::shared_ptr<VertexBuffer> VertexBuffer::GetCubeVertexBuffer() {
	float data[4 * 6 * 5] = {
		-0.5f, -0.5f, -0.5f, 0, 1,
		 0.5f, -0.5f, -0.5f, 1, 1,
		 0.5f, -0.5f,  0.5f, 1, 0,
		-0.5f, -0.5f,  0.5f, 0, 0,

		-0.5f,  0.5f, -0.5f, 0, 0,
		 0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f,  0.5f, 1, 1,
		-0.5f,  0.5f,  0.5f, 1, 0,

		 0.5f, -0.5f, -0.5f, 1, 0, 
		-0.5f, -0.5f, -0.5f, 0, 0,
		-0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f, -0.5f, 1, 1,

		 0.5f, -0.5f,  0.5f, 0, 0, 
		-0.5f, -0.5f,  0.5f, 1, 0,
		-0.5f,  0.5f,  0.5f, 1, 1,
		 0.5f,  0.5f,  0.5f, 0, 1,

		-0.5f, -0.5f, -0.5f, 1, 0,
		-0.5f,  0.5f, -0.5f, 1, 1,
		-0.5f,  0.5f,  0.5f, 0, 1,
		-0.5f, -0.5f,  0.5f, 0, 0,

		 0.5f, -0.5f, -0.5f, 0, 0, 
		 0.5f,  0.5f, -0.5f, 0, 1,
		 0.5f,  0.5f,  0.5f, 1, 1,
		 0.5f, -0.5f,  0.5f, 1, 0,
	};

	return std::make_shared<VertexBuffer>(
		data,
		sizeof(data),
		VertexBufferLayout {
			{ "vertices", 1, ShaderDataType::Float3 },
			{ "texCoords", 1, ShaderDataType::Float2 }
		}
	);
}
