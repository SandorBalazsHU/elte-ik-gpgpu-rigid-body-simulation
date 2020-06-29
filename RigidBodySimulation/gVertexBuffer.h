#pragma once

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

#include <list>
#include <map>
#include <vector>

class gVertexBuffer
{
public:
	gVertexBuffer(void);
	~gVertexBuffer(void);

	void AddAttribute(int _idx, int _comps);
	
	/// _idx: the attribute channel on which data is being supplied
	void AddData(int _idx, float _x);
	void AddData(int _idx, float _x, float _y);
	void AddData(int _idx, float _x, float _y, float _z);
	void AddData(int _idx, float _x, float _y, float _z, float _w);
	void AddData(int _idx, glm::vec4& _v);
	void AddData(int _idx, glm::vec3& _v);
	void AddData(int _idx, glm::vec2& _v);
	void AddIndex(unsigned int _a);
	void AddIndex(unsigned int _a, unsigned int _b);
	void AddIndex(unsigned int _a, unsigned int _b, int unsigned _c);
	void AddIndex(unsigned int _a, unsigned int _b, int unsigned _c, int unsigned _d);

	void SetPatchVertices(int _n);

	void Draw(GLenum _mode, GLint _first, GLsizei _count);
	void DrawIndexed(GLenum _mode, GLint _first, GLsizei _count, const GLvoid* _indices);

	void InitBuffers();

	void On();
	void Off();

	void Clean();

	struct AttribDesc
	{
		AttribDesc() : idx(0), comps(1), stride(0), usage(GL_STATIC_DRAW) {}
		AttribDesc(int _idx, int _comps) : idx(_idx), comps(_comps), usage(GL_STATIC_DRAW)
		{
			stride = _comps*sizeof(GL_FLOAT);
		}
		AttribDesc(int _idx, int _comps, int _stride) : idx(_idx), comps(_comps), stride(_stride), usage(GL_STATIC_DRAW) {}
		AttribDesc(int _idx, int _comps, int _stride, int _usage) : idx(_idx), comps(_comps), stride(_stride), usage(_usage) {}
		int	idx;
		int comps;
		int stride;  
		int usage;
	};

protected:
	GLuint	m_vao;

	// associate an attribute descriptor struct with a channel index
	std::map< int, AttribDesc >	m_attribs;

	std::map< int, std::vector<float> >	m_values;
	std::vector<unsigned int> m_indices;

	std::list<GLuint>	m_vbo_ids;
};

