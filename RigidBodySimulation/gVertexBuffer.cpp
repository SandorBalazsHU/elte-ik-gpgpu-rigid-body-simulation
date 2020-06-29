#include "gVertexBuffer.h"

gVertexBuffer::gVertexBuffer(void)
{
	m_vao = 0;
}


gVertexBuffer::~gVertexBuffer(void)
{
}

void gVertexBuffer::Clean()
{
	m_indices.clear();
	m_attribs.clear();
	m_values.clear();
	for( std::list<GLuint>::iterator it = m_vbo_ids.begin(); it != m_vbo_ids.end(); ++it)
	{
		glDeleteBuffers(1, &(*it) );
	}
	glDeleteVertexArrays(1, &m_vao);
	m_vbo_ids.clear();
}

void gVertexBuffer::AddAttribute(int _idx, int _comps)
{
	AttribDesc desc(_idx, _comps);
	m_attribs[_idx] = desc;
}

void gVertexBuffer::AddData(int _idx, float _x)
{
	m_values[_idx].push_back(_x);
}

void gVertexBuffer::AddData(int _idx, float _x, float _y)
{
	m_values[_idx].push_back(_x);
	m_values[_idx].push_back(_y);
}

void gVertexBuffer::AddData(int _idx, float _x, float _y, float _z)
{
	m_values[_idx].push_back(_x);
	m_values[_idx].push_back(_y);
	m_values[_idx].push_back(_z);
}

void gVertexBuffer::AddData(int _idx, float _x, float _y, float _z, float _w)
{
	m_values[_idx].push_back(_x);
	m_values[_idx].push_back(_y);
	m_values[_idx].push_back(_z);
	m_values[_idx].push_back(_w);
}

void gVertexBuffer::AddData(int _idx, glm::vec2& _v)
{
	m_values[_idx].push_back(_v.x);
	m_values[_idx].push_back(_v.y);
}

void gVertexBuffer::AddData(int _idx, glm::vec3& _v)
{
	m_values[_idx].push_back(_v.x);
	m_values[_idx].push_back(_v.y);
	m_values[_idx].push_back(_v.z);
}

void gVertexBuffer::AddData(int _idx, glm::vec4& _v)
{
	m_values[_idx].push_back(_v.x);
	m_values[_idx].push_back(_v.y);
	m_values[_idx].push_back(_v.z);
	m_values[_idx].push_back(_v.w);
}

void gVertexBuffer::InitBuffers()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// for each attribute, create a VBO and fill it with the data and set the VAO attribs
	for ( std::map<int, AttribDesc>::iterator it = m_attribs.begin(); it != m_attribs.end(); ++it)
	{
		GLuint vbo_id = 0;
		glGenBuffers(1, &vbo_id);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(	GL_ARRAY_BUFFER,									// dest
						sizeof(GL_FLOAT)*m_values[it->second.idx].size(),	// size
						&(m_values[it->second.idx][0]),						// source
						it->second.usage );									// usage

		glVertexAttribPointer(	it->second.idx,		// channel
								it->second.comps,	// number of components
								GL_FLOAT,			// type
								GL_FALSE,			// normalized flag
								it->second.stride,	// stride
								0);					// offset

		glEnableVertexAttribArray(it->second.idx);

		m_vbo_ids.push_back( vbo_id );
	}

	// if indices are specified, upload the element array to the GPU
	if ( m_indices.size() > 0 )
	{
		GLuint index_buffer = 0;
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*m_indices.size(), &(m_indices[0]), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gVertexBuffer::On()
{
	glBindVertexArray(m_vao);
}

void gVertexBuffer::Off()
{
	glBindVertexArray(0);
}

void gVertexBuffer::AddIndex(unsigned int _a)
{
	m_indices.push_back(_a);
}

void gVertexBuffer::AddIndex(unsigned int _a, unsigned int _b)
{
	m_indices.push_back(_a);
	m_indices.push_back(_b);
}

void gVertexBuffer::AddIndex(unsigned int _a, unsigned int _b, unsigned int _c)
{
	m_indices.push_back(_a);
	m_indices.push_back(_b);
	m_indices.push_back(_c);
}

void gVertexBuffer::AddIndex(unsigned int _a, unsigned int _b, unsigned int _c, unsigned int _d)
{
	m_indices.push_back(_a);
	m_indices.push_back(_b);
	m_indices.push_back(_c);
	m_indices.push_back(_d);
}

void gVertexBuffer::SetPatchVertices(int _n)
{
	glPatchParameteri(GL_PATCH_VERTICES, _n);
}
void gVertexBuffer::Draw(GLenum _mode, GLint _first, GLsizei _count)
{
	glDrawArrays(_mode, _first, _count);
}
void gVertexBuffer::DrawIndexed(GLenum _mode, GLint _first, GLsizei _count, const GLvoid* _indices = 0)
{
	glDrawElements(_mode, _count, GL_UNSIGNED_INT, _indices);
}