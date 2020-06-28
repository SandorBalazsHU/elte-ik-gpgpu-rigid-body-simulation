#include "ProgramObject.h"

#include <iostream>

ProgramObject::ProgramObject()
{
	m_id = glCreateProgram();
}


ProgramObject::~ProgramObject()
{
	Clean();

	if (m_id != 0)
		glDeleteShader(m_id);
}

ProgramObject::ProgramObject(std::initializer_list<ShaderObject> shaderList, std::initializer_list<Binding> attribLocationBindingList, std::initializer_list<Binding> fragDataBindingList)
{
	m_id = glCreateProgram();
	Init(shaderList, attribLocationBindingList, fragDataBindingList);
}

ProgramObject::ProgramObject(ProgramObject && rhs)
{
	m_id = rhs.m_id;
	m_list_shaders_attached = std::move(rhs.m_list_shaders_attached);
	m_map_uniform_locations = std::move(rhs.m_map_uniform_locations);

	rhs.m_id = 0;
}

ProgramObject & ProgramObject::operator=(ProgramObject && rhs)
{
	if (&rhs == this)
		return *this;

	m_id = rhs.m_id;
	m_list_shaders_attached = std::move(rhs.m_list_shaders_attached);
	m_map_uniform_locations = std::move(rhs.m_map_uniform_locations);

	rhs.m_id = 0;

	return *this;
}

void ProgramObject::Init(std::initializer_list<ShaderObject> shaderList, std::initializer_list<Binding> attribLocationBindingList, std::initializer_list<Binding> fragDataBindingList)
{
	if (m_id != 0)
		Clean();

	AttachShaders(shaderList);
	BindAttribLocations(attribLocationBindingList);
	BindFragDataLocations(fragDataBindingList);
	LinkProgram();
}

void ProgramObject::Clean()
{
	for (auto shader : m_list_shaders_attached)
		glDetachShader(m_id, shader);
}

ProgramObject& ProgramObject::AttachShader(const ShaderObject & shader)
{
	glAttachShader(m_id, shader);
	m_list_shaders_attached.push_back(shader);
	return *this;
}

ProgramObject& ProgramObject::AttachShaders(std::initializer_list<ShaderObject> shaderList)
{
	for (const ShaderObject& shader : shaderList)
	{
		glAttachShader(m_id, shader);
		m_list_shaders_attached.push_back(shader);
	}
	return *this;
}

void ProgramObject::BindAttribLocation(int _index, const char * _inVariableName) const
{
	glBindAttribLocation(m_id, _index, _inVariableName);
}

void ProgramObject::BindAttribLocations(std::initializer_list<std::pair<int, const char*>> bindingList) const
{
	for (const auto& binding : bindingList)
	{
		glBindAttribLocation(m_id, binding.first, binding.second);
	}
}

void ProgramObject::BindFragDataLocation(int _index, const char * _outVariableName) const
{
	glBindAttribLocation(m_id, _index, _outVariableName);
}

void ProgramObject::BindFragDataLocations(std::initializer_list<Binding> fragDataBindingList) const
{
	for (const auto& binding : fragDataBindingList)
	{
		glBindAttribLocation(m_id, binding.first, binding.second);
	}
}

bool ProgramObject::LinkProgram()
{
	if (m_id == 0)
		return false;

	glLinkProgram(m_id);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_id, GL_LINK_STATUS, &result);
	glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result)
	{
		GLchar* error = new char[infoLogLength];
		glGetProgramInfoLog(m_id, infoLogLength, nullptr, error);
		std::cerr << "[Link] Hiba: " << error;
		delete[] error;

		return false;
	}

	return true;
}

GLint ProgramObject::GetLocation(const char * _uniform)
{
	auto loc_it = m_map_uniform_locations.find(_uniform);
	if (loc_it == m_map_uniform_locations.end())
	{
		GLint my_loc = glGetUniformLocation(m_id, _uniform);
		m_map_uniform_locations[_uniform] = my_loc;
		return my_loc;
	}
	else
		return loc_it->second;
}

void ProgramObject::Use() const
{
	glUseProgram(m_id);
}

void ProgramObject::Unuse() const
{
	glUseProgram(0);
}

void ProgramObject::SetTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glUniform1i(GetLocation(_uniform), _sampler);
}

void ProgramObject::SetCubeTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	glUniform1i(GetLocation(_uniform), _sampler);
}
GLint ProgramObject::GLResolveUniformLocation(GLint _uniform) 
{ 
	return _uniform; 
}

GLint ProgramObject::GLResolveUniformLocation(const char* _uniform) 
{ 
	return GetLocation(_uniform); 
}
