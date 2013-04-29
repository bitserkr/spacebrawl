#ifndef _AABB_H
#define _AABB_H

#include "math/vector3.h"

namespace bbk
{
struct AABB
{
	Point3  center;
	Vector3 diag;

	AABB(const Point3& inCenter = Point3(), const Vector3& inDiag = Vector3()) : center(inCenter), diag(inDiag) {}
}; // struct AABB
} // namespace bbk

#endif /* _AABB_H */