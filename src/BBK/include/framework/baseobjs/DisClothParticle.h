#ifndef _DISCLOTHPARTICLE_H
#define _DISCLOTHPARTICLE_H

#include "graphics/rendercontext.h"

namespace bbk
{
struct DisClothParticle
{
	DisClothParticle();
	void Integrate(float deltatime);
	void Update(float deltatime);
	void Draw();
	void SetStatic(bool bStatic);
	void ApplyNettForce(const Vector3& nettForce);

	/** @name
	 *  State vector *///\{
	Vector3 pos;
	Vector3 mom;
	Vector3 vel;
	Vector3 force;
	float invMass;
	//\}
	Vector3 nettF;

	RenderContext renderContext;
	bool bStatic;

	struct NeighbourInfo
	{
		DisClothParticle* part;
		float restDist;
		float springConst;
		float dampCoeff;

		NeighbourInfo() : part(nullptr), restDist(0.5f), springConst(10.0f), dampCoeff(1.0f) {}
	};
	NeighbourInfo left, right, bottom, top;
}; // class DisClothParticle
} // namespace bbk

#endif /* _DISCLOTHPARTICLE_H */
