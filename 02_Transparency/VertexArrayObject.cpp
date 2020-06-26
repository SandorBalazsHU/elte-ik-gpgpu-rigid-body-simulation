#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject()
{
	glGenVertexArrays(1, &m_id);
}

VertexArrayObject::~VertexArrayObject()
{
	if (m_id != 0)
	{
		glDeleteVertexArrays(1, &m_id);
		m_id = 0;
	}
}

VertexArrayObject::VertexArrayObject(VertexArrayObject && rhs)
{
	if (&rhs == this)
		return;

	m_id = rhs.m_id;
	rhs.m_id = 0;
}

VertexArrayObject & VertexArrayObject::operator=(VertexArrayObject && rhs)
{
	if (&rhs == this)
		return *this;

	m_id = rhs.m_id;
	rhs.m_id = 0;

	return *this;
}

VertexArrayObject& VertexArrayObject::Bind()
{
	glBindVertexArray(m_id);
	return *this;
}

void VertexArrayObject::Unbind()
{
	glBindVertexArray(0);
}

VertexArrayObject& VertexArrayObject::SetIndices(const IndexBuffer& pIndexBuffer)
{
	Bind();
	pIndexBuffer.Bind();
	return *this;
}

void VertexArrayObject::Init(std::initializer_list<std::pair<AttributeData, const ArrayBuffer&>> pDataBuffers)
{
	Bind();
	for (auto val : pDataBuffers)
	{
		val.second.Bind();
		val.first.Apply();
	}
}

void VertexArrayObject::Init(std::initializer_list<std::pair<AttributeData, const ArrayBuffer&>> pDataBuffers, const IndexBuffer& pIndexBuffer)
{
	Init(pDataBuffers);
	pIndexBuffer.Bind();
	Unbind();
}