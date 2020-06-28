#pragma once

// Based on sroccaserra's (https://sourceforge.net/users/sroccaserra/) Obj loader
// http://sourceforge.net/projects/objloader/

#include <fstream>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "Mesh_OGL3.h"

#include <memory>

class ObjParser
{
public:
	static std::unique_ptr<Mesh> parse(const char* fileName);

	enum Exception { EXC_FILENOTFOUND };
private:
	struct IndexedVert {
		int v, vt, vn;
		IndexedVert(int _v, int _vt, int _vn) : v(_v), vt(_vt), vn(_vn) {};
		bool operator<(const IndexedVert& rhs) const { 
			return v<rhs.v || (v == rhs.v && (vt<rhs.vt || (vt == rhs.vt && vn<rhs.vn)));
		}
	};
		
	ObjParser(void) : mesh(0), nIndexedVerts(0) {}

	bool processLine();
	bool skipCommentLine();
	void skipLine();
	void addIndexedVertex(const IndexedVert& vertex);

	Mesh* mesh;
	std::ifstream ifs;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;

	unsigned int nIndexedVerts;
	std::map<IndexedVert, unsigned int> vertexIndices;
};
