#ifndef _TRIANGLE_H
#define _TRIANGLE_H

namespace bbk
{
struct Vertex;

struct Triangle
{
	Vertex *v0, *v1, *v2;

	Triangle(Vertex *vtx0=nullptr, Vertex *vtx1=nullptr, Vertex *vtx2=nullptr) : v0(vtx0), v1(vtx1), v2(vtx2) {}
};
} // namespace bbk

#endif /* _TRIANGLE_H */
