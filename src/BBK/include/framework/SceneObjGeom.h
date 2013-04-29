#ifndef _SCENEOBJGEOM_H
#define _SCENEOBJGEOM_H

#include <string>
#include "math/tuple.h"

namespace bbk
{
struct Vector3;

struct SceneObjGeom
{
	std::string name;
	size_t   numVertices;
	size_t*  indices;
	Vector3* positions;
	Vector3* normals;
	Tuple<float, 2>* texcoords;
}; // struct SceneObjGeom
} // namespace bbk

#endif /* _SCENEOBJGEOM_H */
