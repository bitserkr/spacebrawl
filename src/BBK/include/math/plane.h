#ifndef _PLANE_H
#define _PLANE_H

#include "vector3.h"

namespace bbk
{
struct Plane
{
	Vector3 nrm;
	float   dist;

	Plane(const Vector3 &normal = Vector3(0.0f, 0.0f, 1.0f), float distance = 0.0f) :
		nrm(normal), dist(distance) {}
}; // struct Plane
} // namespace bbk

#endif /* _PLANE_H */
