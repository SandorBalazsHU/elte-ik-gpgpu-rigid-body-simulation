template<BufferType type, BufferUsage usage>
inline VertexArrayObject & VertexArrayObject::AddAttribute(AttributeData& pAttrib, BufferObject<type, usage>& pBuffer)
{
	pBuffer.Bind();
	pAttrib.Apply();

	return *this;
}