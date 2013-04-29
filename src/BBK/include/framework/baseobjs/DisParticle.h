#ifndef _DISPARTICLE_H
#define _DISPARTICLE_H

#include "math/vector3.h"

namespace bbk
{
struct DisParticle
{
	DisParticle() : invMass(1.0f) {}
	void Integrate(float deltatime);
	void Update(float deltatime);
	void Draw()const ;

	Vector3 pos;
	Vector3 mom;
	Vector3 vel;
	Vector3 force;
	float invMass;
}; // class DisParticle
} // namespace bbk

#endif /* _DISPARTICLE_H */
