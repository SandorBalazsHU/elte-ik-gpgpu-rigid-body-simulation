#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

#include <utility>
#include <string>
#include <vector>
#include <array>

#include <type_traits>

/*

	Convenience conversion functions to simplify the creation of ShaderObject objects using the _vs, _tcs, _tes, _gs, _fs, and _comp suffices. 

*/
using TypeSourcePair = std::pair<GLuint, std::string>;

inline TypeSourcePair operator"" _vs  (const char * str, size_t len) { return std::make_pair(GL_VERTEX_SHADER,			std::string{ str, len }); }
inline TypeSourcePair operator"" _tcs (const char * str, size_t len) { return std::make_pair(GL_TESS_CONTROL_SHADER,	std::string{ str, len }); }
inline TypeSourcePair operator"" _tes (const char * str, size_t len) { return std::make_pair(GL_TESS_EVALUATION_SHADER, std::string{ str, len }); }
inline TypeSourcePair operator"" _gs  (const char * str, size_t len) { return std::make_pair(GL_GEOMETRY_SHADER,		std::string{ str, len }); }
inline TypeSourcePair operator"" _fs  (const char * str, size_t len) { return std::make_pair(GL_FRAGMENT_SHADER,		std::string{ str, len }); }
inline TypeSourcePair operator"" _comp(const char * str, size_t len) { return std::make_pair(GL_COMPUTE_SHADER,			std::string{ str, len }); }

/*

	Enabler templates to restruct SetUniform utility function instantiations to supported types only.

*/
template <typename T>
using IsValidBaseType = std::enable_if_t<
	std::is_same_v<T, GLint> ||
	std::is_same_v<T, GLuint> ||
	std::is_same_v<T, GLfloat> ||
	std::is_same_v<T, GLdouble>
>;

#ifdef GLM_VERSION
	template <typename T, glm::precision P = glm::defaultp>
	using IsValidGLMVectorType = std::enable_if_t<
		std::is_base_of_v<glm::tvec1<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tvec2<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tvec3<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tvec4<typename T::value_type, P>, T>
	>;

	template <typename T, glm::precision P = glm::defaultp>
	using IsValidGLMMatrixType = std::enable_if_t<
		std::is_base_of_v<glm::tmat2x2<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat2x3<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat2x4<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat3x2<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat3x4<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat3x4<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat4x2<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat4x3<typename T::value_type, P>, T> ||
		std::is_base_of_v<glm::tmat4x4<typename T::value_type, P>, T>
	>;
#endif

/*

	Convert types to OpenGL type GLenum values listed in https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml

*/

template <typename T> constexpr GLenum NativeTypeToOpenGLType() { static_assert(true, "Non-supported vertex attribute type"); }
template <> constexpr GLenum NativeTypeToOpenGLType<float>() { return GL_FLOAT; }
template <> constexpr GLenum NativeTypeToOpenGLType<double>() { return GL_DOUBLE; }

// we have to provide these extraction functions because tvec<*,*>::length(), cols(), rows() are not defined as a constexpr...
template<typename T>	constexpr std::pair<size_t, size_t> ComponentCount() { return std::make_pair(1, 1); }
#ifdef GLM_VERSION
	#define COMPONENT_COUNT_SPECIALIZATION(TYPE, DIM_X, DIM_Y) template<> constexpr std::pair<size_t, size_t> ComponentCount<TYPE>() { return std::make_pair(DIM_X, DIM_Y); }
	COMPONENT_COUNT_SPECIALIZATION(glm::vec2, 2, 1)
	COMPONENT_COUNT_SPECIALIZATION(glm::vec3, 3, 1)
	COMPONENT_COUNT_SPECIALIZATION(glm::vec4, 4, 1)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat2, 2, 2)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat3, 3, 3)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat4, 4, 4)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat4x3, 4, 3)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat4x2, 4, 2)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat3x2, 3, 2)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat3x4, 3, 4)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat2x3, 2, 3)
	COMPONENT_COUNT_SPECIALIZATION(glm::mat2x4, 2, 4)
#endif
/*

	Extract primitive type of argument, i.e. what is the base type (e.g. float) of whatever construct (e.g. glm::vec3, glm::mat4, etc.)
	we received. 
	
*/

template <typename T, typename C = void> struct GLExtractPrimitiveType;
template <typename T> struct GLExtractPrimitiveType<T, IsValidBaseType<T>>		{ using primitive_type = T;};
#ifdef GLM_VERSION
	template <typename T> struct GLExtractPrimitiveType<T, IsValidGLMVectorType<T>> { using primitive_type = typename T::value_type; };
	template <typename T> struct GLExtractPrimitiveType<T, IsValidGLMMatrixType<T>> { using primitive_type = typename T::value_type; };
#endif

/*

	Selection of appropriate OpenGL glUniform* function, depending on argument primitive type.
	
*/
template <typename T, int N, int M> inline void CallSetter(GLint, GLsizei, const T*) { }

#define CALL_SETTER_VECTOR_SPECIALIZATION(TYPE, DIM_X, FUNCTION)		template <> inline void CallSetter<TYPE, DIM_X, 1>(GLint pLoc, GLsizei pCount, const TYPE* pArg) { FUNCTION(pLoc, pCount, pArg); }
#define CALL_SETTER_MATRIX_SPECIALIZATION(TYPE, DIM_X, DIM_Y, FUNCTION) template <> inline void CallSetter<TYPE, DIM_X, DIM_Y>(GLint pLoc, GLsizei pCount, const TYPE* pArg) { FUNCTION(pLoc, pCount, GL_FALSE, pArg); }

CALL_SETTER_VECTOR_SPECIALIZATION(GLint,		1, glUniform1iv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLint,		2, glUniform2iv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLint,		3, glUniform3iv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLint,		4, glUniform4iv	)

CALL_SETTER_VECTOR_SPECIALIZATION(GLuint,		1, glUniform1uiv)
CALL_SETTER_VECTOR_SPECIALIZATION(GLuint,		2, glUniform2uiv)
CALL_SETTER_VECTOR_SPECIALIZATION(GLuint,		3, glUniform3uiv)
CALL_SETTER_VECTOR_SPECIALIZATION(GLuint,		4, glUniform4uiv)

CALL_SETTER_VECTOR_SPECIALIZATION(GLfloat,		1, glUniform1fv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLfloat,		2, glUniform2fv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLfloat,		3, glUniform3fv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLfloat,		4, glUniform4fv	)

CALL_SETTER_VECTOR_SPECIALIZATION(GLdouble,		1, glUniform1dv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLdouble,		2, glUniform2dv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLdouble,		3, glUniform3dv	)
CALL_SETTER_VECTOR_SPECIALIZATION(GLdouble,		4, glUniform4dv	)

CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 4, 4, glUniformMatrix4fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 3, 3, glUniformMatrix3fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 2, 2, glUniformMatrix2fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 4, 3, glUniformMatrix4x3fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 4, 2, glUniformMatrix4x2fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 3, 4, glUniformMatrix3x4fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 2, 4, glUniformMatrix2x4fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 3, 2, glUniformMatrix3x2fv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLfloat, 2, 3, glUniformMatrix2x3fv)

CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	4, 4, glUniformMatrix4dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	3, 3, glUniformMatrix3dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	2, 2, glUniformMatrix2dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	4, 3, glUniformMatrix4x3dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	4, 2, glUniformMatrix4x2dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	3, 4, glUniformMatrix3x4dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	2, 4, glUniformMatrix2x4dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	3, 2, glUniformMatrix3x2dv)
CALL_SETTER_MATRIX_SPECIALIZATION(GLdouble,	2, 3, glUniformMatrix2x3dv)

/*

	Extract the type of the argument elements. In case of a scalar, this means the type of the argument. In case
	of containers this, boils down to the type of the array elements.

*/
template <typename T>			struct ElementInfo { using value_type = T; };
template <typename T>			struct ElementInfo<std::vector<T>> { using value_type = T; };
template <typename T, size_t N> struct ElementInfo<std::array<T, N>> { using value_type = T; };
template <typename T, size_t N> struct ElementInfo<T[N]> { using value_type = T; };

/*

	Compute the length of the container in _elements_ in types that our SetUniform supports. 

*/
template <typename T>			constexpr GLsizei ContainerLength(const T&) { return 1; }
template <typename T, size_t N> constexpr GLsizei ContainerLength(const std::array<T, N>&) { return N; }
template <typename T, size_t N> constexpr GLsizei ContainerLength(const T(&)[N]) { return N; }
template <typename T>			constexpr GLsizei ContainerLength(const std::vector<T>& pArr) { return (GLsizei)pArr.size(); }

/*

Compute the length of the container in _bytes_.

*/
template <typename T>			constexpr GLsizei ContainerSizeInBytes(const T&) { return sizeof(T); }
template <typename T, size_t N> constexpr GLsizei ContainerSizeInBytes(const std::array<T, N>&) { return N*sizeof(T); }
template <typename T, size_t N> constexpr GLsizei ContainerSizeInBytes(const T(&)[N]) { return N*sizeof(T); }
template <typename T>			constexpr GLsizei ContainerSizeInBytes(const std::vector<T>& pArr) { return (GLsizei)pArr.size()*sizeof(T); }

/*

	Get a pointer to the beginning of the container. In case of non-contagious containers this should involve
	the creation of a contagious copy of the contents of the passed in containers. 

*/
template <typename T>			constexpr const T* PointerToStart(const T& pArg) { return &pArg; }
template <typename T, size_t N> constexpr const T* PointerToStart(const std::array<T, N>& pArg) { return &pArg[0]; }
template <typename T, size_t N> constexpr const T* PointerToStart(const T(&pArg)[N]) { return &pArg[0]; }
template <typename T>			constexpr const T* PointerToStart(const std::vector<T>& pArg) { return pArg.empty() ? nullptr: &pArg[0]; }

/*

	Helper constexpr to determine if the supplied parameter is a supported contagious container or not.

*/

template<typename T> struct HasContiguousStorage : public std::false_type { };

template<typename T> struct HasContiguousStorage<std::vector<T>> : public std::true_type {};
template<typename T, size_t N> struct HasContiguousStorage<std::array<T, N>> : public std::true_type {};
template<typename T, size_t N> struct HasContiguousStorage< T[N]> : public std::true_type {};

template <typename T, typename R = void> 
using IsContiguousContainer = std::enable_if_t<HasContiguousStorage< T>::value, R>;