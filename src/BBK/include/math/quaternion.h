#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "vector3.h"

namespace bbk
{
typedef struct Quaternion Quat;
struct Quaternion
{
	Quaternion(float scalar=1.0f, const Vector3& vec=Vector3()) : s(scalar), v(vec) {}
	Quaternion(const Quaternion& rhs) : s(rhs.s), v(rhs.v) {}

	Quaternion& operator= (const Quaternion& rhs)       {s=rhs.s; v=rhs.v; return *this;}
	Quaternion  operator+ (const Quaternion& rhs) const {return Quat(s+rhs.s, v+rhs.v);}
	Quaternion& operator+=(const Quaternion& rhs)       {s+=rhs.s; v+=rhs.v; return *this;}
	Quaternion  operator- (const Quaternion& rhs) const {return Quat(s-rhs.s, v-rhs.v);}
	Quaternion& operator-=(const Quaternion& rhs)       {s-=rhs.s; v-=rhs.v; return *this;}
	Quaternion  operator* (const Quaternion& rhs) const {return Quat(s*rhs.s - v.Dot(rhs.v), s*rhs.v + rhs.s*v + v.Cross(rhs.v));}
	Quaternion& operator*=(const Quaternion& rhs)       {*this = this->operator*(rhs); return *this;}

	friend Quaternion  operator* (float f, const Quat& q) {return Quat(f*q.s, f*q.v);}
	friend Quaternion  operator* (const Quat& q, float f) {return f*q;}
	friend Quaternion& operator*=(Quat& q, float f) {q = f*q; return q;}
	friend Quaternion  operator/ (const Quat& q, float f) {float m = 1.0f / f; return m*q;}

	float MagnitudeSq() const {return s*s + v.MagnitudeSq();}
	float Magnitude()   const {return std::sqrt(MagnitudeSq());}
	Quat Normalise()    const {float m = Magnitude(); if (m < 0.000001f) return *this; float invm = 1.0f/m; return Quat(invm*s, invm*v);}
	void NormaliseThis()      {float m = Magnitude(); if (m < 0.000001f) return; float invm = 1.0f / m; this->operator*=(invm);}

	Quaternion Conjugate() const {return Quat(s, -v);}
	Quaternion Inverse()   const {return Conjugate() / Magnitude();}

	static Quaternion MakeRotation(const Vector3& axis, float angle_rad) {const Vector3 n(axis.Normalise()); return Quat(std::cos(angle_rad*0.5f), std::sin(angle_rad*0.5f)*n);}
	static Quaternion MakeRotation(const Vector3& axisAngle) {const float m = axisAngle.Magnitude(); return MakeRotation(axisAngle, m);}

	Vector3 RotateVec(const Vector3& vec) {return (2.0f*s*s - 1.0f)*vec + (2.0f*v.Dot(vec))*v + (2.0f*s)*v.Cross(vec);}

	float   s;
	Vector3 v;
}; // struct Quaternion
} // namespace bbk

#endif /* _QUATERNION_H */
