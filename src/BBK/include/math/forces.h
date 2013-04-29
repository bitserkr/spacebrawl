#ifndef _FORCES_H
#define _FORCES_H

#include "vector3.h"

namespace bbk
{
struct RigidBody
{
	// State vars
	Vector3 pos;
	Vector3 mom;

	// Derivatives
	Vector3 vel;
	Vector3 force;

	float mass;
}; // struct RigidBody

struct DerivVector
{
	DerivVector(const Vector3& inVel = Vector3(), const Vector3& inForce = Vector3()) : vel(inVel), force(inForce) {}

	friend DerivVector operator*(float f, const DerivVector& v) {return DerivVector(v.vel*f, v.force*f);}
	friend DerivVector operator*(const DerivVector& v, float f) {return f * v;}

	Vector3 vel;
	Vector3 force;
}; // struct DerivVector

struct StateVector
{
	StateVector(const Vector3& inPos = Vector3(), const Vector3& inMom = Vector3()) : pos(inPos), mom(inMom) {}

	friend StateVector operator*(float f, const StateVector& v) {return StateVector(v.pos*f, v.mom*f);}
	friend StateVector operator*(const StateVector& v, float f) {return f * v;}
	//friend StateVector operator+(const DerivVector& dv) {return StateVector(pos + dv.vel, mom + dv.force);}

	Vector3 pos;
	Vector3 mom;
}; // struct StateVector

void HookesLaw(RigidBody& rb, float springConst, float dampCoeff);
void Centripetal(RigidBody& rb, const Point3& origin);
DerivVector HookesLaw(const StateVector& v, float springConst, float dampCoeff, float invMass);
DerivVector Centripetal(const StateVector& v, const Point3& origin, float mass);
Vector3 HookesLaw(float springConst, const Point3& disp, float dampCoeff, const Vector3& vel);
Vector3 Centripetal(const Point3& origin, const Point3& pos, const Vector3& vel, float mass);
} // namespace bbk

#endif /* _FORCES_H */
