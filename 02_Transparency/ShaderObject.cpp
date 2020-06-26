#include "ShaderObject.h"

#include <iostream>
#include <fstream>

ShaderObject::ShaderObject(GLenum pType)
{
	m_id = glCreateShader(pType);
}

ShaderObject::~ShaderObject()
{
	if (m_id != 0)
	{
		glDeleteShader(m_id);
		m_id = 0;
	}
}

ShaderObject::ShaderObject(ShaderObject &&rhs)
{
	m_id = rhs.m_id;
	rhs.m_id = 0;
}

ShaderObject & ShaderObject::operator=(ShaderObject &&rhs)
{
	if (&rhs == this)
		return *this;

	m_id = rhs.m_id;
	rhs.m_id = 0;

	return *this;
}

ShaderObject::ShaderObject(GLenum pType, const std::string &pFilenameOrSource)
{
	m_id = glCreateShader(pType);

	if (!FromFile(pType, pFilenameOrSource.c_str()))
	{
		if ( !std::ifstream(pFilenameOrSource).good())
			FromMemory(pType, pFilenameOrSource);
	}
}

bool ShaderObject::FromFile(GLenum _shaderType, const char* _filename)
{
	// _fileName megnyitasa
	std::ifstream shaderStream(_filename);

	if (!shaderStream.is_open())
		return false;

	// shaderkod betoltese _fileName fajlbol
	std::string shaderCode = "";

	// file tartalmanak betoltese a shaderCode string-be
	std::string line = "";
	while (std::getline(shaderStream, line))
		shaderCode += line + "\n";

	shaderStream.close();

	// térjünk vissza a fordítás eredményével
	return CompileShaderFromMemory(m_id, shaderCode) > 0;
}

bool ShaderObject::FromMemory(GLenum _shaderType, const std::string& _source)
{
	CompileShaderFromMemory(m_id, _source);
	if (m_id != 0)
		return true;
	else
		return false;
}

GLuint ShaderObject::CompileShaderFromMemory(const GLuint _shaderObject, const std::string& _source)
{
	// betoltott kod hozzarendelese a shader-hez
	const char* sourcePointer = _source.c_str();
	glShaderSource(_shaderObject, 1, &sourcePointer, nullptr);

	// shader leforditasa
	glCompileShader(_shaderObject);

	// ellenorizzuk, h minden rendben van-e
	GLint result = GL_FALSE;
	int infoLogLength;

	// forditas statuszanak lekerdezese
	glGetShaderiv(_shaderObject, GL_COMPILE_STATUS, &result);
	glGetShaderiv(_shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (GL_FALSE == result)
	{
		GLchar* error = new char[infoLogLength];
		glGetShaderInfoLog(_shaderObject, infoLogLength, nullptr, error);

		std::cerr << "Hiba: " << error << std::endl;

		delete[] error;

		return 0;
	}

	return _shaderObject;
}