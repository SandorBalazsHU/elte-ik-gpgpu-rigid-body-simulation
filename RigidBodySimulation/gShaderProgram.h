#pragma once

#include <GL/glew.h>
#include <SDL_opengl.h>

#include <map>
#include <string>
#include <list>

#include <glm\glm.hpp>

class gShaderProgram
{
public:
	gShaderProgram(void);
	~gShaderProgram(void);

	bool AttachShader(GLenum _shaderType, const char* _filename);
	void BindAttribLoc(int _index, const char* _uniform);
	void BindFragDataLoc(int _index, const char* _uniform);
	bool LinkProgram();

	void SetVerbose(bool);

	void On();
	void Off();

	void Clean();

	void SetUniform(const char* _uniform, glm::vec2& _vec);
	void SetUniform(const char* _uniform, glm::vec3& _vec);
	void SetUniform(const char* _uniform, glm::vec4& _vec);
	void SetUniform(const char* _uniform, glm::mat4& _mat);
	void SetUniform(const char* _uniform, int _i);
	void SetUniform(const char* _uniform, float _f);
	void SetTexture(const char* _uniform, int _sampler, GLuint _textureID);
	void SetCubeTexture(const char* _uniform, int _sampler, GLuint _textureID);
protected:
	GLuint	getLocation(const char* _uniform);
	GLuint	loadShader(GLenum _shaderType, const char* _fileName);
	std::string includeShaderCode(std::string);//const char* _fileName);

	GLuint							m_id_program;
	std::map< std::string, GLint >	m_map_uniform_locations;
	std::list< GLuint >				m_list_shaders_attached;

	bool	m_verbose;
};

