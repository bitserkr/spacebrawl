#include "framework/baseobjs/camera.h"
#include "math/mathlib.h"
#include "graphics/graphics.h"

namespace bbk
{
Camera::Camera() :
	pos_(Vector3(0.0f, 0.0f, 0.0f)),
	viewVec_(Vector3(0.0f, 0.0f, -1.0f)),
	rightVec_(Vector3(1.0f, 0.0f, 0.0f)),
	globUp_(Vector3(0.0f, 1.0f, 0.0f)),
	rollAngle_(0.0f),
	azimuth_(bbk::PIf),
	inclin_(0.0f),
	radius_(1.0f),
	l_(0.0f),
	r_(0.0f),
	b_(0.0f),
	t_(0.0f),
	n_(0.0f),
	f_(0.0f),
	vertFoVDeg_(0.0f),
	aspectRatio_(0.0f),
	isDirty_(true)
{
}

Camera::~Camera() {}

void Camera::Pitch(float angle)
{
	SetInclin(inclin_ + angle);
}

void Camera::Yaw(float angle)
{
	SetAzimuth(azimuth_ + angle);
}

void Camera::Roll(float angle)
{
	rollAngle_ += angle;
}

void Camera::OrbitalDisp(const Vector3 &pivot, float dAzimuth, float dInclin)
{
	// Construct vector from pivot point to camera position
	Vector3 disp(pos_ - pivot);

	// Convert disp vector to spherical coords
	float radius  = disp.Magnitude();
	float inclin  = std::asin(disp.y / radius) + dInclin;
	float azimuth = std::atan2(-disp.z, disp.x) + dAzimuth;

	// Convert spherical coords back to vec
	disp.y = radius * std::sin(inclin);
	float base = radius * std::cos(inclin);
	disp.x = base * std::cos(azimuth);
	disp.z = -(base * std::sin(azimuth));

	SetPosition(pivot + disp);
}

void Camera::OrbitalDispAndLook(const Vector3 &pivot, float dAzimuth, float dInclin)
{
	// Construct vector from pivot point to camera position
	Vector3 disp(pos_ - pivot);

	// Convert disp vector to spherical coords
	float radius  = disp.Magnitude();
	if (radius < 0.000001f)
	{
		Pitch(-dInclin);
		Yaw(dAzimuth);
		return;
	}
	float inclin  = std::asin(disp.y / radius) + dInclin;
	float azimuth = std::atan2(-disp.z, disp.x) + dAzimuth;

	// Convert spherical coords back to vec
	disp.y = radius * std::sin(inclin);
	float base = radius * std::cos(inclin);
	disp.x = base * std::cos(azimuth);
	disp.z = -(base * std::sin(azimuth));

	pos_ = pivot + disp;
	SetViewVector(-disp);

	gfx::DrawLine(pos_, Colour(0.0f, 0.0f, 1.0f, 0.64f), pivot, Colour(1.0f, 1.0f, 1.0f, 0.64f));
}

void Camera::DrawFrustum() const
{
	Vector3 negz(viewVec_.Normalise());
	Vector3 y(rightVec_.Cross(negz).Normalise());

	Vector3 nearplane(n_ * negz);
	Point3 nbl(b_ * y + l_ * rightVec_ + nearplane);
	Point3 nbr(b_ * y + r_ * rightVec_ + nearplane);
	Point3 ntl(t_ * y + l_ * rightVec_ + nearplane);
	Point3 ntr(t_ * y + r_ * rightVec_ + nearplane);

	float mult = f_ / n_;
	Point3 fbl(nbl * mult);
	Point3 fbr(nbr * mult);
	Point3 ftl(ntl * mult);
	Point3 ftr(ntr * mult);

	nbl += pos_; nbr += pos_; ntl += pos_; ntr += pos_;
	fbl += pos_; fbr += pos_; ftl += pos_; ftr += pos_;

	const Colour colour(0.0f, 1.0f, 0.0f, 0.64f);
	// Draw nearplane
	gfx::DrawLine(nbl, colour, nbr, colour);
	gfx::DrawLine(nbr, colour, ntr, colour);
	gfx::DrawLine(ntr, colour, ntl, colour);
	gfx::DrawLine(ntl, colour, nbl, colour);
	
	// Draw farplane
	gfx::DrawLine(fbl, colour, fbr, colour);
	gfx::DrawLine(fbr, colour, ftr, colour);
	gfx::DrawLine(ftr, colour, ftl, colour);
	gfx::DrawLine(ftl, colour, fbl, colour);

	// Draw frustum edges
	gfx::DrawLine(pos_, colour, fbl, colour);
	gfx::DrawLine(pos_, colour, fbr, colour);
	gfx::DrawLine(pos_, colour, ftl, colour);
	gfx::DrawLine(pos_, colour, ftr, colour);

	// Highlight frustum sides
	/*Colour fade(colour);
	fade.a *= 0.5f;
	DrawQuad(Vertex(nbr, fade, 0.0f, 1.0f), Vertex(nbl, fade, 1.0f, 1.0f), Vertex(fbl, fade, 1.0f, 0.0f), Vertex(fbr, fade, 0.0f, 0.0f));
	DrawQuad(Vertex(ntl, fade, 0.0f, 1.0f), Vertex(ntr, fade, 1.0f, 1.0f), Vertex(ftr, fade, 1.0f, 0.0f), Vertex(ftl, fade, 0.0f, 0.0f));
	DrawQuad(Vertex(nbl, fade, 0.0f, 1.0f), Vertex(ntl, fade, 1.0f, 1.0f), Vertex(ftl, fade, 1.0f, 0.0f), Vertex(fbl, fade, 0.0f, 0.0f));
	DrawQuad(Vertex(ntr, fade, 0.0f, 1.0f), Vertex(nbr, fade, 1.0f, 1.0f), Vertex(fbr, fade, 1.0f, 0.0f), Vertex(ftr, fade, 0.0f, 0.0f));
	DrawQuad(Vertex(nbl, fade, 0.0f, 1.0f), Vertex(nbr, fade, 1.0f, 1.0f), Vertex(ntr, fade, 1.0f, 0.0f), Vertex(ntl, fade, 0.0f, 0.0f));
	DrawQuad(Vertex(fbr, fade, 0.0f, 1.0f), Vertex(fbl, fade, 1.0f, 1.0f), Vertex(ftl, fade, 1.0f, 0.0f), Vertex(ftr, fade, 0.0f, 0.0f));*/

	// Draw camera axes
	{
		gfx::DrawLine(
			pos_,
			bbk::Colour(1.0f, 0.0f, 0.0f, 1.0f),
			pos_ + bbk::Vector3(mtx44_worldToView_.elements[0], mtx44_worldToView_.elements[4], mtx44_worldToView_.elements[8]),
			bbk::Colour(1.0f, 0.0f, 0.0f, 1.0f));
		gfx::DrawLine(
			pos_,
			bbk::Colour(0.0f, 1.0f, 0.0f, 1.0f),
			pos_ + bbk::Vector3(mtx44_worldToView_.elements[1], mtx44_worldToView_.elements[5], mtx44_worldToView_.elements[9]),
			bbk::Colour(0.0f, 1.0f, 0.0f, 1.0f));
		gfx::DrawLine(
			pos_,
			bbk::Colour(0.0f, 0.0f, 1.0f, 1.0f),
			pos_ + bbk::Vector3(mtx44_worldToView_.elements[2], mtx44_worldToView_.elements[6], mtx44_worldToView_.elements[10]),
			bbk::Colour(0.0f, 0.0f, 1.0f, 1.0f));
	}
}

void Camera::GetPlaneDists(float planeDists[6]) const
{
	planeDists[0] = l_;
	planeDists[1] = r_;
	planeDists[2] = b_;
	planeDists[3] = t_;
	planeDists[4] = n_;
	planeDists[5] = f_;
}

void Camera::UpdateVecFromSpherical()
{
	// Height of view vector
	viewVec_.y = radius_ * std::sin(inclin_);
	// Project view unto xz-plane
	float base = radius_ * std::cos(inclin_);
	// Decompose base into x and z components
	viewVec_.x = base * std::cos(azimuth_);
	viewVec_.z = -(base * std::sin(azimuth_));
}

void Camera::UpdateSphericalFromVec()
{
	radius_  = viewVec_.Magnitude();
	inclin_  = std::asin(viewVec_.y / radius_);
	azimuth_ = std::atan2(-viewVec_.z, viewVec_.x);
}

void Camera::ComputeViewMtx() const
{
	Vector3 w = viewVec_.Normalise();
	Vector3 u = w.Cross(globUp_).Normalise();
	rightVec_ = u;

	/*Matrix4x4 rot = Matrix4x4::MakeRotate(w, rollAngle_);
	u = rot * u;
	u.NormaliseThis();*/
	Vector3 v = (u.Cross(w)).Normalise();

	mtx44_worldToView_ = Matrix4x4(
		u.x, v.x, -w.x, 0.0f,
		u.y, v.y, -w.y, 0.0f,
		u.z, v.z, -w.z, 0.0f,
		-(u.Dot(pos_)), -(v.Dot(pos_)), w.Dot(pos_), 1.0f);

	mtx44_viewToWorld_ = Matrix4x4(
		u.x, u.y, u.z, 0.0f,
		v.x, v.y, v.z, 0.0f,
		-w.x, -w.y, -w.z, 0.0f,
		pos_.x, pos_.y, pos_.z, 1.0f);

	isDirty_ = false;
}

void Camera::UpdatePlaneEqns() const
{
	ComputeViewMtx();
	const Matrix4x4 mtx44_projview = mtx44_proj_ * mtx44_worldToView_;
	const Vector4 r0(mtx44_projview.GetRow(0));
	const Vector4 r1(mtx44_projview.GetRow(1));
	const Vector4 r2(mtx44_projview.GetRow(2));
	const Vector4 r3(mtx44_projview.GetRow(3));

	// Left plane
	{
		Vector4 v = -r0 - r3;
		frustum_.leftPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.leftPl.nrm.Magnitude();
		frustum_.leftPl.nrm *= invmag;
		frustum_.leftPl.dist = -v.w * invmag;
	}
	// Right plane
	{
		Vector4 v = r0 - r3;
		frustum_.rightPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.rightPl.nrm.Magnitude();
		frustum_.rightPl.nrm *= invmag;
		frustum_.rightPl.dist = -v.w * invmag;
	}
	// Bottom plane
	{
		Vector4 v = -r1 - r3;
		frustum_.bottomPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.bottomPl.nrm.Magnitude();
		frustum_.bottomPl.nrm *= invmag;
		frustum_.bottomPl.dist = -v.w * invmag;
	}
	// Top plane
	{
		Vector4 v = r1 - r3;
		frustum_.topPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.topPl.nrm.Magnitude();
		frustum_.topPl.nrm *= invmag;
		frustum_.topPl.dist = -v.w * invmag;
	}
	// Near plane
	{
		Vector4 v = -r2 - r3;
		frustum_.nearPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.nearPl.nrm.Magnitude();
		frustum_.nearPl.nrm *= invmag;
		frustum_.nearPl.dist = -v.w * invmag;
	}
	// Far plane
	{
		Vector4 v = r2 - r3;
		frustum_.farPl.nrm = Vector3(v.x, v.y, v.z);
		float invmag = 1.0f / frustum_.farPl.nrm.Magnitude();
		frustum_.farPl.nrm *= invmag;
		frustum_.farPl.dist = -v.w * invmag;
	}
}
} // namespace bbk
