#include "BufferObject.h"

#include <GL\glew.h>
#include <GL\GL.h>

template <BufferType target, BufferUsage usage>
GLuint BufferObject<target, usage>::g_lastBound = 0;

template<BufferType target, BufferUsage usage>
inline BufferObject<target, usage>::BufferObject()
{
	glGenBuffers(1, &m_id);
}

template<BufferType target, BufferUsage usage>
inline BufferObject<target, usage>::~BufferObject()
{
	Clean();
	m_id = 0;
	m_sizeInBytes = 0;
}

template<BufferType target, BufferUsage usage>
inline void BufferObject<target, usage>::Clean()
{
	if (m_id != 0 && m_sizeInBytes != 0)
	{
		glDeleteBuffers(1, &m_id);
	}
}

template<BufferType target, BufferUsage usage>
inline BufferObject<target, usage>::BufferObject(BufferObject && rhs)
{
	if (&rhs == this)
		return;

	m_id = rhs.m_id;
	rhs.m_id = 0;
}

template<BufferType target, BufferUsage usage>
inline BufferObject<target, usage> & BufferObject<target, usage>::operator=(BufferObject<target, usage> && rhs)
{
	if (&rhs == this)
		return *this;

	Clean();

	m_id = rhs.m_id;
	rhs.m_id = 0;
}

template<BufferType target, BufferUsage usage>
inline void BufferObject<target, usage>::BufferData(GLsizeiptr pSize, const GLvoid * pSource)
{
	Bind();

	glBufferData(static_cast<GLenum>(target), pSize, pSource, static_cast<GLenum>(usage) );
	m_sizeInBytes = pSize;
}

template<BufferType target, BufferUsage usage>
inline void BufferObject<target, usage>::BufferSubData(GLintptr pOffset, GLsizeiptr pSize, const GLvoid * pSource)
{
	Bind();

	glBufferSubData(static_cast<GLenum>(target), pOffset, pSize, pSource);
	m_sizeInBytes = pSize;
}

#include <iostream>

template<BufferType target, BufferUsage usage>
inline void BufferObject<target, usage>::Bind() const
{
	glBindBuffer(static_cast<GLenum>(target), m_id);
	/*
	if (target == BufferType::ElementArray)
		std::cout << "last = " << g_lastBound << ", mine = " << m_id << "\n";

	if (g_lastBound != m_id)
	{		
		glBindBuffer(static_cast<GLenum>(target), m_id);
		g_lastBound = m_id;
	}
	*/
}

template<BufferType target, BufferUsage usage>
template<typename T>
inline BufferObject<target, usage>::BufferObject(const std::vector<T>& pArr)
{
	BufferData(ContainerSizeInBytes(pArr), (const void*)(&(pArr[0])));
}

template<BufferType target, BufferUsage usage>
template<typename T, size_t N>
inline BufferObject<target, usage>::BufferObject(const std::array<T, N>& pArr)
{
	BufferData(ContainerSizeInBytes(pArr), (const void*)(&(pArr[0])));
}

template<BufferType target, BufferUsage usage>
template<typename T>
inline IsContiguousContainer<T> BufferObject<target, usage>::BufferData(const T & pArr)
{
	BufferData(ContainerSizeInBytes(pArr), PointerToStart(pArr));
}

template<BufferType target, BufferUsage usage>
template<typename T>
inline BufferObject<target, usage>& BufferObject<target, usage>::operator=(const T & pArr)
{
	static_assert(HasContiguousStorage<T>::value, "Cannot upload data to OpenGL buffer object: supplied argument is not a known contiguous container.");

	BufferData( ContainerSizeInBytes(pArr), (const void*)(&(pArr[0])) );

	return *this;
}

template<BufferType target, BufferUsage usage>
template<typename T>
inline BufferObject<target, usage>::operator std::vector<T>() const
{
	Bind();

	T* ptr = static_cast<T*>(glMapBuffer(static_cast<GLenum>(target), GL_READ_ONLY));

	std::vector<T> ret{};
	ret.assign(ptr, ptr + m_sizeInBytes / sizeof(T));

	glUnmapBuffer(static_cast<GLenum>(target));

	return ret;
}

template<BufferType target, BufferUsage usage>
template<typename T, size_t N>
inline BufferObject<target, usage>::operator std::array<T, N>() const
{
	Bind();

	T* ptr = static_cast<T*>(glMapBuffer(static_cast<GLenum>(target), GL_READ_ONLY));

	std::array<T, N> ret{};
	const size_t elementCount = m_sizeInBytes / sizeof(T);

	if (elementCount > N)
		std::copy(ptr, ptr + N, ret.begin());
	else
		std::copy(ptr, ptr + m_sizeInBytes / sizeof(T), ret.begin());

	glUnmapBuffer(static_cast<GLenum>(target));

	return ret;
}
