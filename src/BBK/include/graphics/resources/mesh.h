#ifndef _MESH_H
#define _MESH_H

#include <string>
#include "graphics/vertex.h"
#include "intersect/intersect.h"

namespace bbk
{
class Mesh
{
public:
	Mesh();
	~Mesh();

	bool LoadMeshFromFile(const char *filename);

	/**
	 * \name
	 * Mesh name
	 *///\{
	const std::string& GetName() const {return name_;}
	void               SetName(const std::string &name) {name_ = name;}
	//\}

	/**
	 * \name
	 * Vertices
	 *///\{
	unsigned  GetNumVertices()    const {return numVertices_;}
	Vertex*   GetVertexArray()          {return vertices_;}
	unsigned* GetVertIndexArray() const {return vertInd_;}
	//\}

	bool hasTexCoords() const {return hasTexCoords_;}
	bool hasNormals()   const {return hasNormals_;}

	const AABB& GetAABB() const {return aabb_;}
	const OBB&  GetOBB()  const {return obb_;}

private:
	std::string name_;
	unsigned    numVertices_;
	Vertex*     vertices_;
	unsigned*   vertInd_;

	bool        hasTexCoords_;
	bool        hasNormals_;

	AABB        aabb_;
	OBB         obb_;
}; // class Mesh
} // namespace bbk

#endif /* _MESH_H */
