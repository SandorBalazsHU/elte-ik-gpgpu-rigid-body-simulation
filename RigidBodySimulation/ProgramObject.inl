#include <typeinfo>

#include <GL\glew.h>
#include <GL\GL.h>

#include <iostream>

#include "GLconversions.hpp"

template<typename U, typename T>
void ProgramObject::SetUniform(U _uniform, const T& pArr)
{
	using ElementType = typename ElementInfo<T>::value_type;
	using PrimitiveType = typename GLExtractPrimitiveType<ElementType>::primitive_type;
	constexpr std::pair<size_t, size_t> componentCount = ComponentCount<ElementType>();

	CallSetter<PrimitiveType, componentCount.first, componentCount.second>(
			GLResolveUniformLocation(_uniform),
			ContainerLength(pArr),
			(const PrimitiveType*)PointerToStart(pArr)
		);
}