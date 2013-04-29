#ifndef _OBB_H
#define _OBB_H

#include "math/vector3.h"
#include "math/matrix4x4.h"

namespace bbk
{
struct OBB
{
	Vector3   u, v, w;
	Point3    center;
	Vector3   halfExtents;

	OBB(const Vec3& x=Vec3(1.f,0.f,0.f), const Vec3& y=Vec3(0.f,1.f,0.f), const Vec3& z=Vec3(0.f,0.f,1.f), const Vec3& mid=Vec3(0.f,0.f,0.f), const Vec3& halfSpan=Vec3(0.5f,0.5f,0.5f)) : u(x), v(y), w(z), center(mid), halfExtents(halfSpan) {}
	friend OBB operator*(const Matrix4x4& mtx, const OBB& obb) {return OBB(mtx*obb.u, mtx*obb.v, mtx*obb.w, mtx*Vector4(obb.center,1.0f), obb.halfExtents);}
}; // struct OBB
} // namespace bbk

#endif /* _OBB_H */
