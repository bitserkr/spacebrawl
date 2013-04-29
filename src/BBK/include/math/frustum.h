#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "plane.h"

namespace bbk
{
struct Frustum
{
	Plane leftPl, rightPl, bottomPl, topPl, nearPl, farPl;

	Frustum(
		const Plane& l = Plane(),
		const Plane& r = Plane(),
		const Plane& b = Plane(),
		const Plane& t = Plane(),
		const Plane& n = Plane(),
		const Plane& f = Plane()) :
		leftPl(l), rightPl(r), bottomPl(b), topPl(t), nearPl(n), farPl(f) {}
}; // struct Frustum
} // namespace bbk

#endif /* _FRUSTUM_H */
