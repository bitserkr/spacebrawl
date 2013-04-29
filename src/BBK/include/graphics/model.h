#ifndef _MODEL_H
#define _MODEL_H

#include <string>
#include "vertex.h"
#include "intersect/intersect.h"
#include "math/matrix3x3.h"

namespace bbk
{
class Model
{
public:
	Model();
	~Model();

	const std::string& GetName() const {return modelName_;}
	bool LoadGeometryFromFile(const char *filename);

	/** @name
	 *  Vertices and vertex attributes *///\{
	size_t    GetNumVertices()    const {return numVertices_;}
	Vertex*   GetVertexArray()          {return vertices_;}
	size_t    GetNumIndices()     const {return numIndices_;}
	unsigned* GetVertIndArray()   const {return indices_;}
	
	bool hasTexCoords() const {return hasTexCoords_;}
	bool hasNormals()   const {return hasNormals_;}
	//\}

	/** @name
	 *  Bounding Volumes *///\{
	const BSphere& GetBSphere()       const {return bsphere_;}
	const Vector3& GetBSphereOffset() const {return bsphereOffset_;}
	const AABB&    GetAABB()          const {return aabb_;}
	const Vector3& GetAABBOffset()    const {return aabbOffset_;}
	const OBB&     GetOBB()           const {return obb_;}
	const Vector3& GetOBBOffset()     const {return obbOffset_;}
	BVHNode*       GetBVH()                 {return bvhroot_;}
	//\}
	
	/** @name
	 *  Rigid Body Dynamics *///\{
	const Vector3&   GetCenterOfMass()  const {return centerMass_;}
	float            GetMass()          const {return mass_;}
	const Matrix3x3& GetInertiaTensor() const {return inertiaTensor_;}
	//\}

private:
	std::string modelName_;
	size_t      numVertices_;
	Vertex*     vertices_;
	size_t      numIndices_;
	unsigned*   indices_;

	bool hasTexCoords_;
	bool hasNormals_;

	BSphere  bsphere_;
	Vector3  bsphereOffset_;
	AABB     aabb_;
	Vector3  aabbOffset_;
	OBB      obb_;
	Vector3  obbOffset_;
	BVHNode* bvhroot_;

	Vector3 centerMass_;
	float   mass_;
	Matrix3x3 inertiaTensor_;
}; // class Model
} // namespace bbk

#endif /* _MODEL_H */
