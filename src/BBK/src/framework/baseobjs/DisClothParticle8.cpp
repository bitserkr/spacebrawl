#include "baseobjs/DisClothParticle8.h"
#include "graphics/graphics.h"
#include "math/forces.h"

namespace bbk
{
DisClothParticle8::DisClothParticle8() :
	invMass(1.0f),
	bStatic(false)
{}

void DisClothParticle8::Integrate(float deltatime)
{
	const float halfdt = deltatime * 0.5f;
	const float coeff = 1.0f / 6.0f;
	const Vector3 currVel = mom * invMass;

	// Reset derivs
	force = nettF;
	vel = Vector3(0.0f, 0.0f, 0.0f);
	nettF = Vector3();

	// Left neighbour
	if (left.part)
	{
		const float &sprConst = left.springConst;
		const float &dampCoeff = left.dampCoeff;
		const Vector3 gap(pos - left.part->pos);
		const Vector3 restPos = gap.Normalise() * left.restDist + left.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}
	
	// Right neighbour
	if (right.part)
	{
		const float &sprConst = right.springConst;
		const float &dampCoeff = right.dampCoeff;
		const Vector3 gap(pos - right.part->pos);
		const Vector3 restPos = gap.Normalise() * right.restDist + right.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}
	
	// Bottom neighbour
	if (bottom.part)
	{
		const float &sprConst = bottom.springConst;
		const float &dampCoeff = bottom.dampCoeff;
		const Vector3 gap(pos - bottom.part->pos);
		const Vector3 restPos = gap.Normalise() * bottom.restDist + bottom.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}
	
	// Top neighbour
	if (top.part)
	{
		const float &sprConst = top.springConst;
		const float &dampCoeff = top.dampCoeff;
		const Vector3 gap(pos - top.part->pos);
		const Vector3 restPos = gap.Normalise() * top.restDist + top.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}

	// Bottom left neighbour
	if (bl.part)
	{
		const float &sprConst = bl.springConst;
		const float &dampCoeff = bl.dampCoeff;
		const Vector3 gap(pos - bl.part->pos);
		const Vector3 restPos = gap.Normalise() * bl.restDist + bl.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}

	// Bottom right neighbour
	if (br.part)
	{
		const float &sprConst = br.springConst;
		const float &dampCoeff = br.dampCoeff;
		const Vector3 gap(pos - br.part->pos);
		const Vector3 restPos = gap.Normalise() * br.restDist + br.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}

	// Top left neighbour
	if (tl.part)
	{
		const float &sprConst = tl.springConst;
		const float &dampCoeff = tl.dampCoeff;
		const Vector3 gap(pos - tl.part->pos);
		const Vector3 restPos = gap.Normalise() * tl.restDist + tl.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}

	// Top right neighbour
	if (tr.part)
	{
		const float &sprConst = tr.springConst;
		const float &dampCoeff = tr.dampCoeff;
		const Vector3 gap(pos - tr.part->pos);
		const Vector3 restPos = gap.Normalise() * tr.restDist + tr.part->pos;

		if ((restPos - pos).MagnitudeSq() > 0.01)
		{
			Vector3 k1_vel = currVel;
			Vector3 k1_force = HookesLaw(sprConst, pos - restPos, dampCoeff, currVel);
		
			Vector3 k2_vel = currVel + k1_force*halfdt*invMass;
			Vector3 k2_force = HookesLaw(sprConst, (pos + k1_vel*halfdt) - restPos, dampCoeff, k2_vel);

			Vector3 k3_vel = currVel + k2_force*halfdt*invMass;
			Vector3 k3_force = HookesLaw(sprConst, (pos + k2_vel*halfdt) - restPos, dampCoeff, k3_vel);
		
			Vector3 k4_vel = currVel + k3_force*deltatime*invMass;
			Vector3 k4_force = HookesLaw(sprConst, (pos + k3_vel*deltatime) - restPos, dampCoeff, k4_vel);

			const float gapmag = gap.Magnitude();
			if (gapmag > 0.1f && gapmag < 5.0f)
			{
				force += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
				vel += coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);
			}
		}
	}
}

void DisClothParticle8::Update(float deltatime)
{
	// Update state vector
	if (!bStatic)
	{
		pos += vel * deltatime;
		mom += force * deltatime;
	}

	// Update render state
	renderContext.transform = Matrix4x4::MakeTranslate(pos) * Matrix4x4::MakeUniScale(renderContext.scale);
	renderContext.bsphere = bbk::TransformBSphere(renderContext.transform, renderContext.model->GetBSphere());
	const Vector3 scaledOffset(renderContext.model->GetBSphereOffset() * renderContext.scale);
	renderContext.bsphere.center += scaledOffset;
	renderContext.bsphere.radius *= std::fabs(renderContext.scale);
}

void DisClothParticle8::Draw()
{
	gfx::DrawObject(renderContext);

	if (left.part)
	{
		const float dist = (left.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < left.restDist * left.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, left.part->pos, clr);
	}
	if (right.part)
	{
		const float dist = (right.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < right.restDist * right.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, right.part->pos, clr);
	}
	if (bottom.part)
	{
		const float dist = (bottom.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < bottom.restDist * bottom.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, bottom.part->pos, clr);
	}
	if (top.part)
	{
		const float dist = (top.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < top.restDist * top.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, top.part->pos, clr);
	}
	if (bl.part)
	{
		const float dist = (bl.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < bl.restDist * bl.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, bl.part->pos, clr);
	}
	if (br.part)
	{
		const float dist = (br.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < br.restDist * br.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, br.part->pos, clr);
	}
	if (tl.part)
	{
		const float dist = (tl.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < tl.restDist * tl.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, tl.part->pos, clr);
	}
	if (tr.part)
	{
		const float dist = (tr.part->pos - pos).MagnitudeSq();
		Colour clr = (dist < tr.restDist * tr.restDist) ? Colour(0.0f, 1.0f, 0.0f, 0.5f) : Colour(1.0f, 0.0f, 0.0f, 0.5f);
		gfx::DrawLine(pos, clr, tr.part->pos, clr);
	}
}

void DisClothParticle8::SetStatic(bool flag)
{
	bStatic = flag;
}

void DisClothParticle8::ApplyNettForce(const Vector3& nettForce)
{
	nettF = nettForce;
}
} // namespace bbk
