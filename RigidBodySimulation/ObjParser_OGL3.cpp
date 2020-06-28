#include "ObjParser_OGL3.h"

#include <string>

using namespace std;

std::unique_ptr<Mesh> ObjParser::parse(const char* fileName)
{
	ObjParser theParser;

	theParser.ifs.open(fileName, ios::in|ios::binary);
	if (!theParser.ifs)
		throw(EXC_FILENOTFOUND);

	theParser.mesh = new Mesh();

	while(theParser.skipCommentLine()) 
	{
		if (false == theParser.processLine())
			break;
	}

	theParser.ifs.close();

	theParser.mesh->initBuffers();

	return std::make_unique<Mesh>(*theParser.mesh);
}

bool ObjParser::processLine()
{
	string line_id;
	float x, y, z;    

	if (!(ifs >> line_id))
		return false;

	if ("v" == line_id) {	//	vertex data
		ifs >> x >> y >> z;
		positions.push_back(glm::vec3(x, y, z));
	}
	else if ("vt" == line_id) {	// texture data
		ifs >> x >> y;
		texcoords.push_back(glm::vec2(x, y));
	}
	else if ("vn" == line_id) {	// normal data
		ifs >> x >> y >> z;
		if(!ifs.good()) {                     // in case it is -1#IND00
			x = y = z = 0.0;
			ifs.clear();
			skipLine();
		}
		normals.push_back(glm::vec3(x, y, z));
	}
	else if("f" == line_id)	{
		unsigned int iPosition = 0, iTexCoord = 0, iNormal = 0;

		for( unsigned int iFace = 0; iFace < 3; iFace++ )
		{
			ifs >> iPosition;
			if( '/' == ifs.peek() )
			{
				ifs.ignore();

				if( '/' != ifs.peek() )
				{
					ifs >> iTexCoord;
				}

				if( '/' == ifs.peek() )
				{
					ifs.ignore();

					// Optional vertex normal
					ifs >> iNormal;
				}
			}

			addIndexedVertex(IndexedVert(iPosition-1, iTexCoord-1, iNormal-1));
		}
	}
	else
		skipLine();
	return true;
}

void ObjParser::addIndexedVertex(const IndexedVert& vertex)
{
	if (vertexIndices[vertex] == 0) // new vertex
	{
		Mesh::Vertex v;
		v.position = positions[vertex.v];
		if (vertex.vt != -1)
			v.texcoord = texcoords[vertex.vt];
		if (vertex.vn != -1)
			v.normal = normals[vertex.vn];
		
		mesh->addVertex(v);
		mesh->addIndex(nIndexedVerts++);		// 0 based indices
		vertexIndices[vertex] = nIndexedVerts;	// but here 1 based, 0 signals "new one"
	} else {
		mesh->addIndex(vertexIndices[vertex]-1);
	}
}

bool ObjParser::skipCommentLine()
{
	char next;
	while( ifs >> std::skipws >> next ) 
	{
		ifs.putback(next);
		if ('#' == next)
			skipLine();
		else
			return true;
	}
	return false;
}

void ObjParser::skipLine()
{
	char next;
	ifs >> std::noskipws;
	while( (ifs >> next) && ('\n' != next) );
}