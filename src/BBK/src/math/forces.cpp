#include "forces.h"

namespace bbk
{
void HookesLaw(RigidBody& rb, float springConst, float dampCoeff)
{
	rb.force = -springConst * rb.pos - dampCoeff * rb.vel;
}

DerivVector HookesLaw(const StateVector& v, float springConst, float dampCoeff, float invMass)
{
	const Vector3 vel = v.mom * invMass;
	const Vector3 force = -springConst * v.pos - dampCoeff * vel;

	return DerivVector(vel, force);
}

DerivVector Centripetal(const StateVector& v, const Vector3& origin, float mass)
{
	const Vector3 vel = v.mom / mass;
	const Vector3 dir = origin - v.pos;
	float radius = dir.Magnitude();
	float mag = mass * vel.MagnitudeSq() / radius;

	return DerivVector(vel, dir * mag);
}

Vector3 HookesLaw(float springConst, const Point3& pos, float dampCoeff, const Vector3& vel)
{
	return (-springConst * pos) - (dampCoeff * vel);
}

Vector3 Centripetal(const Point3& origin, const Point3& pos, const Vector3& vel, float mass)
{
	const Vector3 dir = origin - pos;
	float radius = dir.Magnitude();
	float mag = mass * vel.MagnitudeSq() / radius;
	return dir.Normalise() * mag;
}
} // namespace bbk
