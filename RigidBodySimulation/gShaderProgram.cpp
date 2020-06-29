#include "gShaderProgram.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
gShaderProgram::gShaderProgram(void) : m_map_uniform_locations(), m_list_shaders_attached(), m_id_program(0)
{
}


gShaderProgram::~gShaderProgram(void)
{
}

bool gShaderProgram::AttachShader( GLenum _shaderType,  const char* _filename )
{
	GLuint loaded_shader = loadShader(_shaderType, _filename);
	if ( loaded_shader == 0 )
		return false;

	m_list_shaders_attached.push_back( loaded_shader );

	if ( m_id_program == 0 )
	{
		m_id_program = glCreateProgram();
		if ( m_id_program == 0 )
		{
			if ( m_verbose )
			{
				std::cout << "Hiba a shader program létrehozásakor" << std::endl;
			}
			return false;
		}
	}

	glAttachShader( m_id_program, loaded_shader );

	return true;
}

bool gShaderProgram::LinkProgram()
{
	if ( m_id_program == 0 )
		return false;

	glLinkProgram(m_id_program);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_id_program, GL_LINK_STATUS, &result);
	glGetProgramiv(m_id_program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result)
	{
		if ( m_verbose )
		{
			std::vector<char> ProgramErrorMessage( infoLogLength );
			glGetProgramInfoLog(m_id_program, infoLogLength, NULL, &ProgramErrorMessage[0]);
			fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		}
		return false;
	}

	return true;
}

// töröljük a shader programot, elõtte detach-oljuk a shadereket
void gShaderProgram::Clean()
{
	// töröljük a már linkelt linkelt shadereket
	for (	std::list<GLuint>::iterator _it = m_list_shaders_attached.begin();
			_it != m_list_shaders_attached.end();
			++_it )
	{
		if ( m_id_program != 0 && *_it != 0)
			glDetachShader( m_id_program, *_it );
		if ( *_it != 0 )
			glDeleteShader( *_it );
	}
	glDeleteProgram(m_id_program);
}

void gShaderProgram::SetVerbose( bool _val )
{
	m_verbose = _val;
}

static std::map<std::string, std::string> memo_;

std::string gShaderProgram::includeShaderCode(std::string _fileName)//const char* _fileName)
{
	auto it = memo_.find(_fileName);
	if(it != memo_.end())
		return it->second;

	std::regex e ("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	std::smatch m;

	// shaderkod betoltese _fileName fajlbol
	std::string shaderCode = "";

	// _fileName megnyitasa
	std::ifstream shaderStream(_fileName);

	if ( !shaderStream.is_open() )
	{
		if (m_verbose)
			fprintf(stderr, "Hiba a %s shader fájl betöltésekor!", _fileName.c_str());
		return 0;
	}

	// file tartalmanak betoltese a shaderCode string-be
	std::string line = "";
	while ( std::getline(shaderStream, line) )
	{
		if(std::regex_search (line,m,e))
		{
			std::string include_file = m[1];
			
			auto loaded = includeShaderCode(include_file);
			shaderCode += loaded;
		}
		else
			shaderCode += line + "\n";
	}
	shaderStream.close();

	memo_[_fileName] = shaderCode;

	return shaderCode;
}

GLuint gShaderProgram::loadShader(GLenum _shaderType, const char* _fileName)
{
	// shader azonosito letrehozasa
	GLuint loadedShader = glCreateShader( _shaderType );

	// ha nem sikerult hibauzenet es -1 visszaadasa
	if ( loadedShader == 0 )
	{
		if (m_verbose)
			fprintf(stderr, "Hiba a shader inicializálásakor (glCreateShader)! Fajlnev: %s", _fileName);
		return 0;
	}
	
	// shaderkod betoltese _fileName fajlbol
	std::string shaderCode = includeShaderCode(_fileName);

	// fajlbol betoltott kod hozzarendelese a shader-hez
	const char* sourcePointer = shaderCode.c_str();
	glShaderSource( loadedShader, 1, &sourcePointer, NULL );

	// shader leforditasa
	glCompileShader( loadedShader );

	// ellenorizzuk, h minden rendben van-e
	GLint result = GL_FALSE;
    int infoLogLength;

	// forditas statuszanak lekerdezese
	glGetShaderiv(loadedShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(loadedShader, GL_INFO_LOG_LENGTH, &infoLogLength);

	if ( GL_FALSE == result )
	{
		if (m_verbose)
		{
			// hibauzenet elkerese es kiirasa
			std::vector<char> VertexShaderErrorMessage(infoLogLength);
			glGetShaderInfoLog(loadedShader, infoLogLength, NULL, &VertexShaderErrorMessage[0]);

			std::cout << "fájl: " << _fileName << std::endl;
			std::cout << "Hiba: " << &VertexShaderErrorMessage[0] << std::endl;
			//fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
		}
		loadedShader = 0;
	}

	return loadedShader;
}

void gShaderProgram::BindAttribLoc(int _index, const char* _uniform)
{
	glBindAttribLocation(m_id_program, _index, _uniform);
}

void gShaderProgram::BindFragDataLoc(int _index, const char* _uniform)
{
	glBindFragDataLocation(m_id_program, _index, _uniform);
}

void gShaderProgram::SetUniform(const char* _uniform, glm::vec2& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform2fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, glm::vec3& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform3fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, int _i)
{
	GLint loc = getLocation(_uniform);
	glUniform1i( loc, _i );
}

void gShaderProgram::SetUniform(const char* _uniform, float _f)
{
	GLint loc = getLocation(_uniform);
	glUniform1f( loc, _f );
}

void gShaderProgram::SetUniform(const char* _uniform, glm::vec4& _vec)
{
	GLint loc = getLocation(_uniform);
	glUniform4fv( loc, 1, &_vec[0] );
}

void gShaderProgram::SetUniform(const char* _uniform, glm::mat4& _mat)
{
	GLint loc = getLocation(_uniform);
	glUniformMatrix4fv( loc, 1, GL_FALSE, &(_mat[0][0]) );
}

inline GLuint	gShaderProgram::getLocation(const char* _uniform)
{
	std::map< std::string, int >::iterator loc_it = m_map_uniform_locations.find(_uniform);
	if ( loc_it == m_map_uniform_locations.end() )
	{
		GLint my_loc = glGetUniformLocation(m_id_program, _uniform);
		m_map_uniform_locations[_uniform] = my_loc;
		return my_loc;
	}
	else
		return loc_it->second;
}

void gShaderProgram::On()
{
	glUseProgram(m_id_program);
}

void gShaderProgram::Off()
{
	glUseProgram(0);
}

void gShaderProgram::SetTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	SetUniform(_uniform, _sampler);
}

void gShaderProgram::SetCubeTexture(const char* _uniform, int _sampler, GLuint _textureID)
{
	glActiveTexture(GL_TEXTURE0 + _sampler);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	SetUniform(_uniform, _sampler);
}