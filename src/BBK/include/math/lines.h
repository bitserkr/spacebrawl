#ifndef _LINES_H
#define _LINES_H

#include "vector3.h"

namespace bbk
{
struct Line
{
	Point3  pt;
	Vector3 vec;

	Line(const Point3& origin, const Vector3& dir) : pt(origin), vec(dir) {}
};

struct Ray
{
	Point3  pt;
	Vector3 vec;
	float   start;

	Ray(const Point3& origin, const Vector3& dir, float inStart = 0.0f) : pt(origin), vec(dir), start(inStart) {}
};

struct Segment
{
	Point3  pt;
	Vector3 vec;
	float   start, end;

	Segment(const Point3& origin, const Vector3& dir, float inStart = 0.0f, float inEnd = 1.0f) : pt(origin), vec(dir), start(inStart), end(inEnd) {}
};
} // namespace bbk

#endif /* _LINES_H */
