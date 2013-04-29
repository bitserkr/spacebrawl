#ifndef _VERTEX_H
#define _VERTEX_H

#include "math/tuple.h"
#include "math/vector3.h"
#include "colour.h"

namespace bbk
{
struct Vertex
{
	Point3          pos;
	Colour          clr;
	Tuple<float, 2> tc;
	Vector3         nrm;

	Vertex(
		const Point3 &position = Point3(),
		const Colour &colour = Colour(),
		float u = 0.0f,
		float v = 0.0f,
		const Vector3 &normal = Vector3()) :
	  pos(position), clr(colour), nrm(normal) {tc[0] = u; tc[1] = v;}
}; // struct Vertex
} // namespace bbk

#endif /* _VERTEX_H */
