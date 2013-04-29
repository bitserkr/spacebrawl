#ifndef _INTERSECT_H
#define _INTERSECT_H

#include "math/mathlib.h"
#include "BSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "BVH_node.h"

namespace bbk
{
bool LinevsPlane(const Line& line, const Plane& plane, Point3& intersection);
bool LinevsSphere(const Line& line, const BSphere& sphere, float intersects[2]);
bool LinevsAABB(const Line& line, const AABB& aabb, float intersects[2]);
bool LinevsOBB(const Line& line, const OBB& obb, float intersects[2]);

BSphere TransformBSphere(const Matrix4x4& transform, const BSphere& srcBSphere);
int BSpherevsPlane(const BSphere& bsphere, const Plane& plane);
AABB TransformAABB(const Matrix4x4& transform, const AABB& srcAABB);
int AABBvsPlane(const AABB& aabb, const Plane& plane);
OBB TransformOBB(const Matrix4x4& transform, const OBB& srcOBB);
int OBBvsPlane(const OBB& obb, const Plane& plane);

void DrawBSphereR(const BSphere& bsphere);
void DrawBSphereG(const BSphere& bsphere);
void DrawAABBR(const AABB& aabb);
void DrawAABBG(const AABB& aabb);
void DrawAABBB(const AABB& aabb);
void DrawOBBR(const OBB& obb);
void DrawOBBG(const OBB& obb);
void DrawOBBB(const OBB& obb);

OBB FitOBBToTri(Vector3 *vertices);
OBB FitOBBToVerts(Vector3 *vertices, size_t numVerts);
BVHNode* BuildBVH(Vector3 *vertices, size_t numVerts);
} // namespace bbk

#endif /* _INTERSECT_H */
