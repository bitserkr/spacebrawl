#ifndef _DisClothParticle88_H
#define _DisClothParticle88_H

#include "graphics/rendercontext.h"

namespace bbk
{
struct DisClothParticle8
{
	DisClothParticle8();
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
		DisClothParticle8* part;
		float restDist;
		float springConst;
		float dampCoeff;

		NeighbourInfo() : part(nullptr), restDist(0.5f), springConst(4.0f), dampCoeff(1.0f) {}
	};
	NeighbourInfo left, right, bottom, top, bl, br, tl, tr;
}; // class DisClothParticle8
} // namespace bbk

#endif /* _DisClothParticle88_H */
