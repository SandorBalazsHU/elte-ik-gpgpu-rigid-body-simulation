#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

#include "GLconversions.hpp"
#include "BufferObject.h"

#include <initializer_list>
#include <utility>

struct AttributeData
{
	AttributeData(GLuint pIndex, GLint pSize, GLenum pType, GLboolean pNormalized, GLsizei pStride, void* pPtr) : index(pIndex), size(pSize), type(pType), normalized(pNormalized), stride(pStride), ptr(pPtr) 
	{ }

	void Apply()
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalized, stride, ptr);
	}

	GLuint		index{};
	GLint		size{};
	GLenum		type{};
	GLboolean	normalized{};
	GLsizei		stride{};
	void*		ptr{};
};

// using template variable for ease of notation on call side
template <GLuint index, typename T, size_t pOffsetInBytes = 0, GLsizei pStrideInBytes = sizeof(T)>
AttributeData CreateAttribute = AttributeData(index, sizeof(T) / sizeof(typename GLExtractPrimitiveType<T>::primitive_type), NativeTypeToOpenGLType<typename GLExtractPrimitiveType<T>::primitive_type>(), false, pStrideInBytes, (void*)pOffsetInBytes);
// otherwise, the following template function is more readable and serves as the function equivalent of the above template variable:
/*
template <GLuint index, typename T>
AttributeData CreateAttribute(size_t pOffsetInBytes = 0, GLsizei pStrideInBytes = sizeof(T))
{
	using PrimitiveType = typename GLExtractPrimitiveType<T>::primitive_type;
	return AttributeData(index, sizeof(T) / sizeof(PrimitiveType), NativeTypeToOpenGLType<PrimitiveType>(), false, pStrideInBytes, (void*)pOffsetInBytes);
}
*/


class VertexArrayObject final
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	VertexArrayObject(const VertexArrayObject&) = delete;
	VertexArrayObject& operator=(const VertexArrayObject&) = delete;

	VertexArrayObject(VertexArrayObject&&);
	VertexArrayObject& operator=(VertexArrayObject&&);

	operator unsigned int() const { return m_id;};

	void Init(std::initializer_list< std::pair<AttributeData, const ArrayBuffer&> > pDataBuffers);

	void Init(std::initializer_list< std::pair<AttributeData, const ArrayBuffer&> > pDataBuffers, const IndexBuffer& pIndexBuffer);

	VertexArrayObject& Bind();
	void Unbind();

	template <BufferType type, BufferUsage usage>
	VertexArrayObject& AddAttribute(AttributeData&, BufferObject<type, usage>&);

	VertexArrayObject& SetIndices(const IndexBuffer&);

private:
	GLuint m_id = 0;
};

#include "VertexArrayObject.inl"