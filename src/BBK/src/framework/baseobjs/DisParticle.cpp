#include "baseobjs/DisParticle.h"
#include "graphics/graphics.h"
#include "math/forces.h"

namespace bbk
{
void DisParticle::Integrate(float deltatime)
{
	const float halfdt = deltatime * 0.5f;
	const float mass = 1.0f / invMass;
	const Vector3 currVel = mom * invMass;

	Vector3 k1_force = Centripetal(Vector3(), pos, currVel, mass);
	Vector3 k1_vel = currVel;

	Vector3 k2_force = Centripetal(Vector3(), pos + k1_vel*halfdt, currVel + k1_force*halfdt*invMass, mass);
	Vector3 k2_vel = currVel + k1_force*halfdt*invMass;

	Vector3 k3_force = Centripetal(Vector3(), pos + k2_vel*halfdt, currVel + k2_force*halfdt*invMass, mass);
	Vector3 k3_vel = currVel + k2_force*halfdt*invMass;

	Vector3 k4_force = Centripetal(Vector3(), pos + k3_vel*deltatime, currVel + k3_force*deltatime*invMass, mass);
	Vector3 k4_vel = currVel + k3_force*deltatime*invMass;

	const float coeff = 1.0f / 6.0f;
	//vel += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
	//pos += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
	force = coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
	vel = coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
}

void DisParticle::Update(float deltatime)
{
	pos += vel * deltatime;
	mom += force * deltatime;

	//---------
	// Midpoint
	////Vector3 netforce = HookesLaw(1.0f, pos_, 0.1f, vel_);
	//Vector3 netforce = Centripetal(Vector3(), pos_, vel_, 1.0f);
	//Vector3 accel = netforce * invMass_;

	//// midpoint pos and vel
	//Vector3 midpos = pos_ + (vel_ * halfdt);
	//Vector3 midvel = vel_ + (accel * halfdt);

	////netforce = HookesLaw(1.0f, midpos, 0.1f, midvel);
	//netforce = Centripetal(Vector3(), midpos, midvel, 1.0f);
	//accel = netforce * invMass_;

	//pos_ += midvel * deltatime;
	//vel_ += accel * deltatime;
}

void DisParticle::Draw() const
{
	gfx::DrawPoint(pos, Colour(1.0f, 1.0f, 1.0f, 1.0f));
}
} // namespace bbk
