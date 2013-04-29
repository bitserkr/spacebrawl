#ifndef _BSPHERE_H
#define _BSPHERE_H

#include "math/vector3.h"

namespace bbk
{
struct BSphere
{
	Point3 center;
	float  radius;

	BSphere(const Point3& inCenter = Point3(), float inRadius = 0.0f) : center(inCenter), radius(inRadius) {}
}; // struct BSphere
} // namespace bbk

#endif /* _BSPHERE_H */
