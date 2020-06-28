#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

#include <vector>

#include "GLconversions.hpp"

/*
	BufferType is an enum class that stands for OpenGL bind targets (from https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferData.xhtml - OpenGL 4.6)
*/
enum class BufferType : GLenum
{
	Array				= GL_ARRAY_BUFFER,
	AtomicCounter		= GL_ATOMIC_COUNTER_BUFFER,
	CopyRead			= GL_COPY_READ_BUFFER,
	CopyWrite			= GL_COPY_WRITE_BUFFER,
	DispatchIndirect	= GL_DISPATCH_INDIRECT_BUFFER,
	DrawIndirect		= GL_DRAW_INDIRECT_BUFFER,
	ElementArray		= GL_ELEMENT_ARRAY_BUFFER,
	PixelPack			= GL_PIXEL_PACK_BUFFER,
	PixelUnpack			= GL_PIXEL_UNPACK_BUFFER,
	Query				= GL_QUERY_BUFFER,
	ShaderStorage		= GL_SHADER_STORAGE_BUFFER,
	Texture				= GL_TEXTURE_BUFFER,
	TransformFeedback	= GL_TRANSFORM_FEEDBACK_BUFFER,
	Uniform				= GL_UNIFORM_BUFFER
};

enum class BufferUsage : GLenum
{
	StreamDraw		= GL_STREAM_DRAW,
	StreamRead		= GL_STREAM_READ,
	StreamCopy		= GL_STREAM_COPY,
	StaticDraw		= GL_STATIC_DRAW,
	StaticRead		= GL_STATIC_READ,
	StaticCopy		= GL_STATIC_COPY,
	DynamicDraw		= GL_DYNAMIC_DRAW,
	DynamicRead		= GL_DYNAMIC_READ,
	DynamicCopy		= GL_DYNAMIC_COPY
};

template <BufferType target = BufferType::Array , BufferUsage usage = BufferUsage::StreamDraw>
class BufferObject final
{
public:
	BufferObject();
	~BufferObject();

	BufferObject(const BufferObject&) = delete;
	BufferObject& operator=(const BufferObject&) = delete;

	BufferObject(BufferObject&& rhs);
	BufferObject& operator=(BufferObject&& rhs);

	template <typename T>
	explicit BufferObject(const std::vector<T>&);

	template <typename T, size_t N>
	explicit BufferObject(const std::array<T, N>&);

	operator unsigned int() const { return m_id; }

	void Clean();

	template <typename T>
	IsContiguousContainer<T> BufferData(const T& pArr);

	void BufferData(GLsizeiptr pSize, const GLvoid* pSource = nullptr);

	void BufferSubData(GLintptr pOffset, GLsizeiptr pSize, const GLvoid* pSource = nullptr);

	inline void Bind() const;

	template <typename T>
	BufferObject& operator=(const T& pArr);

	// returns a read-only copy of the buffer contents as an std::vector
	template <typename T>
	operator std::vector<T>() const;

	// returns a read-only copy of the buffer contents as an array
	template <typename T, size_t N>
	operator std::array<T, N>() const;

private:
	GLuint m_id{};
	GLsizeiptr m_sizeInBytes{};
	static GLuint g_lastBound;
};

#include "BufferObject.inl"

using ArrayBuffer = BufferObject<>;
using IndexBuffer = BufferObject<BufferType::ElementArray>;

