#include "intersect.h"
#include "graphics/graphics.h"
#include "graphics/vertex.h"
#include "utils.h"

namespace bbk
{
bool LinevsPlane(const Line& line, const Plane& plane, Point3& intersection)
{
	float denom = plane.nrm.Dot(line.vec);
	
	if (std::fabs(denom) < bbk::EPSILONf)
		return false;

	float numer = plane.dist - plane.nrm.Dot(line.pt);
	float param = numer / denom;

	intersection = line.pt + line.vec*param;
	return true;
}

bool LinevsSphere(const Line& line, const BSphere& sphere, float intersects[2])
{
	// Trivial acceptance
	if ((line.pt - sphere.center).MagnitudeSq() <= sphere.radius*sphere.radius)
	{
		intersects[0] = 0.0f;
		return true;
	}

	Vector3 nrmDir = line.vec.Normalise();
	Vector3 disp   = line.pt - sphere.center;
	double B = 2.0f * disp.Dot(nrmDir);
	double C = disp.Dot(disp) - sphere.radius*sphere.radius;
	double discrim = B*B - 4.0f*C;

	if (discrim < 0.0)
		return false;

	float discrimsqrt = static_cast<float>(std::sqrt(discrim));

	float root0 = 0.5f*(static_cast<float>(-B) + discrimsqrt);
	float root1 = 0.5f*(static_cast<float>(-B) - discrimsqrt);

	intersects[0] = std::min(root0, root1);
	intersects[1] = std::max(root0, root1);
	return true;
}

bool LinevsAABB(const Line& line, const AABB& aabb, float intersects[2])
{
	intersects[0] = 0.0f;
	intersects[1] = FLT_MAX;
	Vector3 min = aabb.center - aabb.diag;
	Vector3 max = aabb.center + aabb.diag;

	// x-axis
	if (std::fabs(line.vec.x) < EPSILONf)
	{
		// Trivial rejection of rays // along x-axis and outside AABB
		if (line.pt.x < min.x || line.pt.x > max.x)
			return false;
	}
	else
	{
		float denom = 1.0f / line.vec.x;
		float timeIn  = (min.x - line.pt.x) * denom;
		float timeOut = (max.x - line.pt.x) * denom;
		if (timeIn > timeOut)
			utils::swap(timeIn, timeOut);
		if (timeIn > intersects[0])
			intersects[0] = timeIn;
		if (timeOut < intersects[1])
			intersects[1] = timeOut;
	}

	// Trivial rejection on non-overlap on any axis
	if (intersects[1] < intersects[0])
		return false;

	// y-axis
	if (std::fabs(line.vec.y) < EPSILONf)
	{
		// Trivial rejection of rays // along y-axis and outside AABB
		if (line.pt.y < min.y || line.pt.y > max.y)
			return false;
	}
	else
	{
		float denom = 1.0f / line.vec.y;
		float timeIn  = (min.y - line.pt.y) * denom;
		float timeOut = (max.y - line.pt.y) * denom;
		if (timeIn > timeOut)
			utils::swap(timeIn, timeOut);
		if (timeIn > intersects[0])
			intersects[0] = timeIn;
		if (timeOut < intersects[1])
			intersects[1] = timeOut;
	}

	// Trivial rejection on non-overlap on any axis
	if (intersects[1] < intersects[0])
		return false;

	// z-axis
	if (std::fabs(line.vec.z) < EPSILONf)
	{
		// Trivial rejection of rays // along x-axis and outside AABB
		if (line.pt.z < min.z || line.pt.z > max.z)
			return false;
	}
	else
	{
		float denom = 1.0f / line.vec.z;
		float timeIn  = (min.z - line.pt.z) * denom;
		float timeOut = (max.z - line.pt.z) * denom;
		if (timeIn > timeOut)
			utils::swap(timeIn, timeOut);
		if (timeIn > intersects[0])
			intersects[0] = timeIn;
		if (timeOut < intersects[1])
			intersects[1] = timeOut;
	}

	// Trivial rejection on non-overlap on any axis
	if (intersects[1] < intersects[0])
		return false;

	return true;
}

bool LinevsOBB(const Line& line, const OBB& obb, float intersects[2])
{
	// Transform line to local and treat it as AABB
	Matrix3x3 worldToLocal(
		obb.u.x, obb.v.x, obb.w.x,
		obb.u.y, obb.v.y, obb.w.y,
		obb.u.z, obb.v.z, obb.w.z);

	Vector3 localVec = worldToLocal * line.vec;
	Vector3 localPt  = worldToLocal * (line.pt - obb.center);
	AABB    localBox(Vector3(), obb.halfExtents);

	return LinevsAABB(Line(localPt, localVec), localBox, intersects);
}

BSphere TransformBSphere(const Matrix4x4& mtx, const BSphere& srcBSphere)
{
	return BSphere(
		Point3(mtx.elements[12], mtx.elements[13], mtx.elements[14]),
		srcBSphere.radius
		);
}

int BSpherevsPlane(const BSphere& bsphere, const Plane& plane)
{
	float dotprod = plane.nrm.Dot(bsphere.center);

	if (dotprod > plane.dist + bsphere.radius)
		return 1;
	else if (dotprod < plane.dist - bsphere.radius)
		return -1;
	return 0;
}

AABB TransformAABB(const Matrix4x4& mtx, const AABB& aabb)
{
	return AABB(
		Point3(
			mtx.elements[12] + mtx.elements[0] * aabb.center.x + mtx.elements[4] * aabb.center.y + mtx.elements[8] * aabb.center.z,
			mtx.elements[13] + mtx.elements[1] * aabb.center.x + mtx.elements[5] * aabb.center.y + mtx.elements[9] * aabb.center.z,
			mtx.elements[14] + mtx.elements[2] * aabb.center.x + mtx.elements[6] * aabb.center.y + mtx.elements[10] * aabb.center.z),
		Vector3(
			std::fabs(mtx.elements[0]) * aabb.diag.x + std::fabs(mtx.elements[4]) * aabb.diag.y + std::fabs(mtx.elements[8]) * aabb.diag.z,
			std::fabs(mtx.elements[1]) * aabb.diag.x + std::fabs(mtx.elements[5]) * aabb.diag.y + std::fabs(mtx.elements[9]) * aabb.diag.z,
			std::fabs(mtx.elements[2]) * aabb.diag.x + std::fabs(mtx.elements[6]) * aabb.diag.y + std::fabs(mtx.elements[10]) * aabb.diag.z)
		);
}

int AABBvsPlane(const AABB& aabb, const Plane& plane)
{
	Point3 negVertex(-aabb.diag);
	if (plane.nrm.x < 0.0f)
		negVertex.x = -negVertex.x;
	if (plane.nrm.y < 0.0f)
		negVertex.y = -negVertex.y;
	if (plane.nrm.z < 0.0f)
		negVertex.z = -negVertex.z;

	if (plane.nrm.Dot(aabb.center + negVertex) > plane.dist)
		return 1; // AABB lies completely outside plane
	
	Point3 posVertex(-negVertex);
	if (plane.nrm.Dot(aabb.center + posVertex) <= plane.dist)
		return -1; // AABB lies completely within plane

	return 0; // AABB straddles plane
}

OBB TransformOBB(const Matrix4x4& mtx, const OBB& obb)
{
	OBB result;
	Matrix3x3 rot(
		mtx.elements[0], mtx.elements[1], mtx.elements[2],
		mtx.elements[4], mtx.elements[5], mtx.elements[6],
		mtx.elements[8], mtx.elements[9], mtx.elements[10]);
	result.u = (rot * obb.u).Normalise();
	result.v = (rot * obb.v).Normalise();
	result.w = (rot * obb.w).Normalise();
	result.center = Vector3(
		mtx.elements[12] + mtx.elements[0] * obb.center.x + mtx.elements[4] * obb.center.y + mtx.elements[8] * obb.center.z,
		mtx.elements[13] + mtx.elements[1] * obb.center.x + mtx.elements[5] * obb.center.y + mtx.elements[9] * obb.center.z,
		mtx.elements[14] + mtx.elements[2] * obb.center.x + mtx.elements[6] * obb.center.y + mtx.elements[10] * obb.center.z);
	result.halfExtents = obb.halfExtents;

	return result;
}

int OBBvsPlane(const OBB& obb, const Plane& plane)
{
	Matrix3x3 globToBB(
		obb.u.x, obb.v.x, obb.w.x,
		obb.u.y, obb.v.y, obb.w.y,
		obb.u.z, obb.v.z, obb.w.z);
	const Vector3 localNrm = globToBB * plane.nrm;
	const Vector3 localCenter = globToBB * obb.center;

	Point3 negVertex(-obb.halfExtents);
	if (localNrm.x < 0.0f)
		negVertex.x = -negVertex.x;
	if (localNrm.y < 0.0f)
		negVertex.y = -negVertex.y;
	if (localNrm.z < 0.0f)
		negVertex.z = -negVertex.z;

	if (localNrm.Dot(localCenter + negVertex) > plane.dist)
		return 1; // OBB lies completely outside plane
	
	Point3 posVertex(-negVertex);
	if (localNrm.Dot(localCenter + posVertex) <= plane.dist)
		return -1; // OBB lies completely within plane

	return 0; // OBB straddles plane
}

void DrawBSphereR(const bbk::BSphere& bsphere)
{
	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(bsphere.center);
	bbk::gfx::MV_Scale(bsphere.radius, bsphere.radius, bsphere.radius);
	bbk::gfx::DrawShape(bbk::gfx::E_SPHERE, bbk::Colour(1.0f, 0.0f, 0.32f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawBSphereG(const bbk::BSphere& bsphere)
{
	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(bsphere.center);
	bbk::gfx::MV_Scale(bsphere.radius, bsphere.radius, bsphere.radius);
	bbk::gfx::DrawShape(bbk::gfx::E_SPHERE, bbk::Colour(0.0f, 1.0f, 0.32f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawAABBR(const bbk::AABB &aabb)
{
	bbk::Point3 v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::Point3 v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(aabb.center);
	bbk::gfx::MV_Scale(aabb.diag.x * 2.0f, aabb.diag.y * 2.0f, aabb.diag.z * 2.0f);
	bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(1.0f, 0.0f, 0.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawAABBG(const bbk::AABB &aabb)
{
	bbk::Point3 v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::Point3 v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(aabb.center);
	bbk::gfx::MV_Scale(aabb.diag.x * 2.0f, aabb.diag.y * 2.0f, aabb.diag.z * 2.0f);
	bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(0.0f, 1.0f, 0.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawAABBB(const bbk::AABB &aabb)
{
	bbk::Point3 v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::Point3 v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y + aabb.diag.y,
		aabb.center.z - aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		aabb.center.x - aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	v1 = bbk::Point3(
		aabb.center.x + aabb.diag.x,
		aabb.center.y - aabb.diag.y,
		aabb.center.z + aabb.diag.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(aabb.center);
	bbk::gfx::MV_Scale(aabb.diag.x * 2.0f, aabb.diag.y * 2.0f, aabb.diag.z * 2.0f);
	bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(0.0f, 0.0f, 1.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawOBBR(const OBB& obb)
{
	const Vector3& u(obb.u.Normalise());
	const Vector3& v(obb.v.Normalise());
	const Vector3& w(obb.w.Normalise());

	Matrix4x4 rot(
		u.x, u.y, u.z, 0.0f,
		v.x, v.y, v.z, 0.0f,
		w.x, w.y, w.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(obb.center);
	bbk::gfx::MV_Push(rot);

	bbk::Point3 v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::Point3 v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f), v1, bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));

	bbk::gfx::MV_Scale(obb.halfExtents.x * 2.0f, obb.halfExtents.y * 2.0f, obb.halfExtents.z * 2.0f);
	//bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(1.0f, 0.0f, 0.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawOBBG(const OBB& obb)
{
	const Vector3& u(obb.u.Normalise());
	const Vector3& v(obb.v.Normalise());
	const Vector3& w(obb.w.Normalise());

	Matrix4x4 rot(
		u.x, u.y, u.z, 0.0f,
		v.x, v.y, v.z, 0.0f,
		w.x, w.y, w.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(obb.center);
	bbk::gfx::MV_Push(rot);

	bbk::Point3 v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::Point3 v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f), v1, bbk::Colour(0.0f, 1.0f, 0.0f, 0.64f));

	bbk::gfx::MV_Scale(obb.halfExtents.x * 2.0f, obb.halfExtents.y * 2.0f, obb.halfExtents.z * 2.0f);
	bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(0.0f, 1.0f, 0.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

void DrawOBBB(const OBB& obb)
{
	const Vector3& u(obb.u.Normalise());
	const Vector3& v(obb.v.Normalise());
	const Vector3& w(obb.w.Normalise());

	Matrix4x4 rot(
		u.x, u.y, u.z, 0.0f,
		v.x, v.y, v.z, 0.0f,
		w.x, w.y, w.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Translate(obb.center);
	bbk::gfx::MV_Push(rot);

	bbk::Point3 v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::Point3 v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		+obb.halfExtents.y,
		-obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	//

	v0 = bbk::Point3(
		-obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	v1 = bbk::Point3(
		+obb.halfExtents.x,
		-obb.halfExtents.y,
		+obb.halfExtents.z);

	bbk::gfx::DrawLine(v0, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), v1, bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));

	bbk::gfx::MV_Scale(obb.halfExtents.x * 2.0f, obb.halfExtents.y * 2.0f, obb.halfExtents.z * 2.0f);
	//bbk::gfx::DrawShape(bbk::gfx::E_CUBE, bbk::Colour(0.0f, 0.0f, 1.0f, 0.16f));
	bbk::gfx::PopMVMatrixStack();
}

OBB FitOBBToTri(Vector3* vertices)
{
	const Vector3 edge01(vertices[1] - vertices[0]);
	const Vector3 edge02(vertices[2] - vertices[0]);
	const Vector3 edge12(vertices[2] - vertices[1]);
	const float mag01 = edge01.MagnitudeSq();
	const float mag02 = edge02.MagnitudeSq();
	const float mag12 = edge12.MagnitudeSq();
	Vector3 longestEdge, perpEdge;

	if (mag01 < mag02 && mag12 < mag02) // edge02 is longest
	{
		longestEdge = edge02.Normalise();
		perpEdge = vertices[1] - (edge01.Dot(longestEdge) * longestEdge + vertices[0]);
	}
	else if (mag01 < mag12 && mag01 < mag12) // edge12 is longest
	{
		longestEdge = edge12.Normalise();
		perpEdge = vertices[0] - (longestEdge.Dot(vertices[0] - vertices[1]) * longestEdge + vertices[1]);
	}
	else // edge01 is longest
	{
		longestEdge = edge01.Normalise();
		perpEdge = vertices[2] - (edge02.Dot(longestEdge) * longestEdge + vertices[0]);
	}

	perpEdge.NormaliseThis();
	const Vector3 w(longestEdge.Cross(perpEdge));
	Vector3 min(longestEdge.Dot(vertices[0]), perpEdge.Dot(vertices[0]), w.Dot(vertices[0]));
	Vector3 max(longestEdge.Dot(vertices[0]), perpEdge.Dot(vertices[0]), w.Dot(vertices[0]));

	float proj0 = vertices[1].Dot(longestEdge);
	if (proj0 < min.x)
		min.x = proj0;
	else if (proj0 > max.x)
		max.x = proj0;

	float proj1 = vertices[1].Dot(perpEdge);
	if (proj1 < min.y)
		min.y = proj1;
	else if (proj1 > max.y)
		max.y = proj1;

	float proj2 = vertices[1].Dot(w);
	if (proj2 < min.z)
		min.z = proj2;
	else if (proj2 > max.z)
		max.z = proj2;

	proj0 = vertices[2].Dot(longestEdge);
	if (proj0 < min.x)
		min.x = proj0;
	else if (proj0 > max.x)
		max.x = proj0;

	proj1 = vertices[2].Dot(perpEdge);
	if (proj1 < min.y)
		min.y = proj1;
	else if (proj1 > max.y)
		max.y = proj1;

	proj2 = vertices[2].Dot(w);
	if (proj2 < min.z)
		min.z = proj2;
	else if (proj2 > max.z)
		max.z = proj2;

	OBB result;
	result.u = longestEdge;
	result.v = perpEdge;
	result.w = w;
	result.center = Matrix3x3(longestEdge, perpEdge, w) * ((min + max) * 0.5f);
	result.halfExtents = (max - min) * 0.5f;
	return result;
}

OBB FitOBBToVerts(Vector3 *vertices, size_t numVerts)
{
	Vector3 u, v, w, min, max;
	{
		Vector3 mean;
		for (size_t i = 0; i < numVerts; ++i)
			mean += vertices[i];
		mean *= 1.0f / numVerts;

		size_t numTri = numVerts / 3;

		float *triAreas = new float[numTri];
		float totalArea = 0.0f;
		bbk::Vector3 *triCentroids = new bbk::Vector3[numTri];
		bbk::Vector3 hullCentroid;

		for (size_t i = 0; i < numTri; ++i)
		{
			bbk::Vector3 edge0 = vertices[3*i + 1] - vertices[3*i];
			bbk::Vector3 edge1 = vertices[3*i + 2] - vertices[3*i];
			triAreas[i] = 0.5f * (edge0.Cross(edge1).Magnitude());
			totalArea += triAreas[i];

			triCentroids[i] = (vertices[3*i] + vertices[3*i + 1] + vertices[3*i + 2]) / 3.0f;
			hullCentroid += triCentroids[i] * triAreas[i];
		}
		hullCentroid /= totalArea;

		bbk::Matrix3x3 covar;
		float result = 0.0f;
		const float invTotArea = 1.0f / (12.0f * totalArea);

		// 0,0
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].x*triCentroids[i].x +
				vertices[3*i].x*vertices[3*i].x +
				vertices[3*i + 1].x*vertices[3*i + 1].x +
				vertices[3*i + 2].x*vertices[3*i + 2].x;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.x * hullCentroid.x;
		covar.elements[0] = result;

		// 1,1
		result = 0.0f;
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].y*triCentroids[i].y +
				vertices[3*i].y*vertices[3*i].y +
				vertices[3*i + 1].y*vertices[3*i + 1].y +
				vertices[3*i + 2].y*vertices[3*i + 2].y;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.y * hullCentroid.y;
		covar.elements[4] = result;

		// 2,2
		result = 0.0f;
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].z*triCentroids[i].z +
				vertices[3*i].z*vertices[3*i].z +
				vertices[3*i + 1].z*vertices[3*i + 1].z +
				vertices[3*i + 2].z*vertices[3*i + 2].z;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.z * hullCentroid.z;
		covar.elements[8] = result;

		// 1,0 & 0,1
		result = 0.0f;
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].y*triCentroids[i].x +
				vertices[3*i].y*vertices[3*i].x +
				vertices[3*i + 1].y*vertices[3*i + 1].x +
				vertices[3*i + 2].y*vertices[3*i + 2].x;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.y * hullCentroid.x;
		covar.elements[1] = result;
		covar.elements[3] = result;

		// 2,0 & 0,2
		result = 0.0f;
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].z*triCentroids[i].x +
				vertices[3*i].z*vertices[3*i].x +
				vertices[3*i + 1].z*vertices[3*i + 1].x +
				vertices[3*i + 2].z*vertices[3*i + 2].x;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.z * hullCentroid.x;
		covar.elements[2] = result;
		covar.elements[6] = result;

		// 2,1 & 1,2
		result = 0.0f;
		for (size_t i = 0; i < numTri; ++i)
		{
			float mult =
				9.0f*triCentroids[i].z*triCentroids[i].y +
				vertices[3*i].z*vertices[3*i].y +
				vertices[3*i + 1].z*vertices[3*i + 1].y +
				vertices[3*i + 2].z*vertices[3*i + 2].y;
			result += triAreas[i] * invTotArea * mult;
		}
		result -= hullCentroid.z * hullCentroid.y;
		covar.elements[5] = result;
		covar.elements[7] = result;

		double eigenvals[3] = {0.0};
		double eigenvecs[3][3] = {{0.0}};
		bbk::Vector3 eigenVecs[3];

		bbk::JacobiSolver3(covar, eigenvals, eigenvecs);
		eigenVecs[0] = bbk::Vector3(static_cast<float>(eigenvecs[0][0]), static_cast<float>(eigenvecs[0][1]), static_cast<float>(eigenvecs[0][2]));
		eigenVecs[1] = bbk::Vector3(static_cast<float>(eigenvecs[1][0]), static_cast<float>(eigenvecs[1][1]), static_cast<float>(eigenvecs[1][2]));
		eigenVecs[2] = bbk::Vector3(static_cast<float>(eigenvecs[2][0]), static_cast<float>(eigenvecs[2][1]), static_cast<float>(eigenvecs[2][2]));

		// Choose the vector with greatest x-coord to be u
		if (std::fabs(eigenVecs[0].x) >= std::fabs(eigenVecs[1].x) &&
			std::fabs(eigenVecs[0].x) >= std::fabs(eigenVecs[2].x))
		{
			u = eigenVecs[0].Normalise();
			
			// Then choose vector with greater y-coord to be v
			v = std::fabs(eigenVecs[1].y) < std::fabs(eigenVecs[2].y) ? eigenVecs[2].Normalise() : eigenVecs[1].Normalise();
		}
		else if (std::fabs(eigenVecs[1].x) > std::fabs(eigenVecs[0].x) &&
			std::fabs(eigenVecs[1].x) > std::fabs(eigenVecs[2].x))
		{
			u = eigenVecs[1].Normalise();

			v = std::fabs(eigenVecs[2].y) < std::fabs(eigenVecs[0].y) ? eigenVecs[0].Normalise() : eigenVecs[2].Normalise();
		}
		else
		{
			u = eigenVecs[2].Normalise();

			v = std::fabs(eigenVecs[1].y) < std::fabs(eigenVecs[0].y) ? eigenVecs[0].Normalise() : eigenVecs[1].Normalise();
		}

		if (u.x < 0.0f)
			u *= -1.0f;
		if (v.y < 0.0f)
			v *= -1.0f;
		w = u.Cross(v).Normalise();

		std::vector<Vector3> vecs(numVerts);
		for (size_t i = 0; i < numVerts; ++i)
			vecs[i] = vertices[i];

		min = Vector3(vertices[0].Dot(u), vertices[0].Dot(v), vertices[0].Dot(w));
		max = Vector3(vertices[0].Dot(u), vertices[0].Dot(v), vertices[0].Dot(w));
		{
			for (size_t i = 1; i < numVerts; ++i)
			{
				// Project vertex onto the 3 axes
				const Vector3& vert = vertices[i];
				const float proj0 = vert.Dot(u);
				if (proj0 < min.x)
					min.x = proj0;
				else if (proj0 > max.x)
					max.x = proj0;

				const float proj1 = vertices[i].Dot(v);
				if (proj1 < min.y)
					min.y = proj1;
				else if (proj1 > max.y)
					max.y = proj1;

				const float proj2 = vertices[i].Dot(w);
				if (proj2 < min.z)
					min.z = proj2;
				else if (proj2 > max.z)
					max.z = proj2;
			}
		}

		delete[] triAreas;
		delete[] triCentroids;
	}
	OBB result;
	result.u = u; result.v = v; result.w = w;
	result.center = Matrix3x3(u, v, w) * ((min + max) * 0.5f); // Convert from obb to model frame
	result.halfExtents = (max - min) * 0.5f;
	return result;
}

BVHNode* BuildBVH(Vector3 *vertices, size_t numVerts)
{
	if (numVerts < 3)
		return nullptr;
	
	if (numVerts == 3) // Build OBB for triangle and return as leaf node
	{
		BVHNode* node = new BVHNode(FitOBBToTri(vertices), nullptr);
		node->numVerts = numVerts;
		node->triVerts = new Vector3[numVerts];
		for (size_t i = 0; i < numVerts; ++i)
			node->triVerts[i] = vertices[i];
		return node;
	}

	if (numVerts % 3)
		return nullptr;

	// Build OBB for input vertices
	OBB currbox(FitOBBToVerts(vertices, numVerts));

	BVHNode *currnode = new BVHNode(currbox);

	// Find splitting plane
	const Vector3 *maxAxis = &currbox.u;
	const Vector3 *midAxis = &currbox.v;
	const Vector3 *minAxis = &currbox.w;

	// u is greatest
	if (currbox.halfExtents.x >= currbox.halfExtents.y &&
		currbox.halfExtents.x >= currbox.halfExtents.z   )
	{
		maxAxis = &currbox.u;
		if (currbox.halfExtents.y > currbox.halfExtents.z)
		{
			midAxis = &currbox.v;
			minAxis = &currbox.w;
		}
		else
		{
			midAxis = &currbox.w;
			minAxis = &currbox.v;
		}
	}
	// v is greatest
	else if ( (currbox.halfExtents.y >= currbox.halfExtents.x) &&
			(currbox.halfExtents.y >= currbox.halfExtents.z)   )
	{
		maxAxis = &currbox.v;
		if (currbox.halfExtents.x > currbox.halfExtents.z)
		{
			midAxis = &currbox.u;
			minAxis = &currbox.w;
		}
		else
		{
			midAxis = &currbox.w;
			minAxis = &currbox.u;
		}
	}
	// w is greatest
	else if ( (currbox.halfExtents.z >= currbox.halfExtents.x) &&
			(currbox.halfExtents.z >= currbox.halfExtents.y)   )
	{
		maxAxis = &currbox.w;
		if (currbox.halfExtents.x > currbox.halfExtents.y)
		{
			midAxis = &currbox.u;
			minAxis = &currbox.v;
		}
		else
		{
			midAxis = &currbox.v;
			minAxis = &currbox.u;
		}
	}

	std::vector<Vector3*> leftChildren, rightChildren;
	// Try max axis
	{
		const float onethird = 1.0f / 3.0f;
		float mean = 0.0f;
		for (size_t i = 0; i < numVerts; i+=3)
		{
			mean += maxAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);
		}
		mean /= static_cast<float>(numVerts / 3);

		// Sort vertices(grouped into 3's to represent triangles) into +ve or -ve half-spaces
		for (size_t i = 0; i < numVerts; i+=3)
		{
			float baryproj = maxAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);

			if (baryproj < mean)
			{
				leftChildren.push_back(vertices + i);
				leftChildren.push_back(vertices + i+1);
				leftChildren.push_back(vertices + i+2);
			}
			else
			{
				rightChildren.push_back(vertices + i);
				rightChildren.push_back(vertices + i+1);
				rightChildren.push_back(vertices + i+2);
			}
		}
	}

	// Try mid axis if uneven
	if ( (leftChildren.size() == 0) || (rightChildren.size() == 0) )
	{
		leftChildren.clear();
		rightChildren.clear();

		float mean = 0.0f;
		// Compute centroid
		for (size_t i = 0; i < numVerts; i+=3)
		{
			const float onethird = 1.0f / 3.0f;
			mean += midAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);
		}
		mean /= static_cast<float>(numVerts / 3);

		// Sort vertices(grouped into 3's to represent triangles) into +ve or -ve half-spaces
		for (size_t i = 0; i < numVerts; i+=3)
		{
			const float onethird = 1.0f / 3.0f;
			float baryproj = midAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);

			if (baryproj < mean)
			{
				leftChildren.push_back(vertices + i);
				leftChildren.push_back(vertices + i+1);
				leftChildren.push_back(vertices + i+2);
			}
			else
			{
				rightChildren.push_back(vertices + i);
				rightChildren.push_back(vertices + i+1);
				rightChildren.push_back(vertices + i+2);
			}
		}
	}

	// Try min axis if uneven
	if ( (leftChildren.size() == 0) || (rightChildren.size() == 0) )
	{
		leftChildren.clear();
		rightChildren.clear();

		float mean = 0.0f;
		// Compute centroid
		for (size_t i = 0; i < numVerts; i+=3)
		{
			const float onethird = 1.0f / 3.0f;
			mean += minAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);
		}
		mean /= static_cast<float>(numVerts / 3);

		// Sort vertices(grouped into 3's to represent triangles) into +ve or -ve half-spaces
		for (size_t i = 0; i < numVerts; i+=3)
		{
			const float onethird = 1.0f / 3.0f;
			float baryproj = minAxis->Dot((vertices[i] + vertices[i+1] + vertices[i+2])*onethird);

			if (baryproj < mean)
			{
				leftChildren.push_back(vertices + i);
				leftChildren.push_back(vertices + i+1);
				leftChildren.push_back(vertices + i+2);
			}
			else
			{
				rightChildren.push_back(vertices + i);
				rightChildren.push_back(vertices + i+1);
				rightChildren.push_back(vertices + i+2);
			}
		}
	}

	Vector3 *leftVerts = new Vector3[leftChildren.size()];
	for (size_t i = 0, size = leftChildren.size(); i < size; ++i)
	{
		leftVerts[i] = *leftChildren[i];
	}
	currnode->left = BuildBVH(leftVerts, leftChildren.size());
	delete[] leftVerts;

	Vector3 *rightVerts = new Vector3[rightChildren.size()];
	for (size_t i = 0, size = rightChildren.size(); i < size; ++i)
	{
		rightVerts[i] = *rightChildren[i];
	}
	currnode->right = BuildBVH(rightVerts, rightChildren.size());
	delete[] rightVerts;

	currnode->numVerts = numVerts;
	currnode->triVerts = new Vector3[numVerts];
	for (size_t i = 0; i < numVerts; ++i)
		currnode->triVerts[i] = vertices[i];
	return currnode;
}
} // namespace bbk
