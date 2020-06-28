#pragma once

#include <GL\glew.h>
#include <GL\GL.h>

#include <string>
#include <utility>

#include "GLconversions.hpp"

class ShaderObject final
{
public:
	ShaderObject(GLenum pType);
	ShaderObject(GLenum pType, const std::string&);
	ShaderObject(const TypeSourcePair& pInfo) : ShaderObject(pInfo.first, pInfo.second) {}

	~ShaderObject();

	ShaderObject(const ShaderObject&)				= delete;
	ShaderObject& operator=(const ShaderObject&)	= delete;

	ShaderObject(ShaderObject&&);
	ShaderObject& operator=(ShaderObject&&);

	operator unsigned int() const { return m_id; }

	bool FromFile(GLenum _shaderType, const char* _filename);
	bool FromMemory(GLenum _shaderType, const std::string& _source);
private:
	GLuint	CompileShaderFromMemory(const GLuint _shaderObject, const std::string& _source);

	GLuint	m_id;
};