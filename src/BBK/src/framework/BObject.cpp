#include <cstdio>
#include "BObject.h"
#include "graphics/graphics.h"
#include "math/conversions.h"

extern bool bUpdateLinear;
extern bool bUpdateAngular;

namespace bbk
{
BObject::BObject() : invMass_(1.0f), overrideQuat_(false)
{
	orient_ = Quat(1.0f, Vector3(0.0f, 0.0f, 0.0f));
}

void BObject::Integrate(float deltatime)
{
	// Aux vars
	const float halfdt = deltatime * 0.5f;
	const Vector3 currVel = linMom_ * invMass_;

	Vector3 k1_vel = currVel;
	Vector3 k1_force = (0.5f * k1_vel.MagnitudeSq() * 0.47f) * -k1_vel.Normalise();

	Vector3 k2_vel = currVel + k1_force*halfdt*invMass_;
	Vector3 k2_force = (0.5f * k2_vel.MagnitudeSq() * 0.47f) * -k2_vel.Normalise();
	
	Vector3 k3_vel = currVel + k2_force*halfdt*invMass_;
	Vector3 k3_force = (0.5f * k3_vel.MagnitudeSq() * 0.47f) * -k3_vel.Normalise();
	
	Vector3 k4_vel = currVel + k3_force*deltatime*invMass_;
	Vector3 k4_force = (0.5f * k4_vel.MagnitudeSq() * 0.47f) * -k4_vel.Normalise();
	
	const float coeff = 1.0f / 6.0f;
	// Average force over time interval added to external nett force
	force_  += coeff*(k1_force + 2.0f*k2_force + 2.0f*k3_force + k4_force);
	// Average velocity over time interval
	linVel_ = coeff*(k1_vel + 2.0f*k2_vel + 2.0f*k3_vel + k4_vel);

	// Angular component
	const Vector3 dAngMom = torque_ * deltatime;
	const Matrix3x3 currRot(QuatToMatrix(orient_));
	const Matrix3x3 currInvI(currRot * invBodyInertiaTensor_ * Matrix3x3::MakeTranspose(currRot));
	const Vector3 currAngVel = currInvI * (angMom_ + dAngMom);
	angVel_ = Quat(0.0f, currAngVel);
}

void BObject::Update(float deltatime)
{
	// Update state vector
	if (bUpdateLinear)
		pos_    += linVel_ * deltatime;
	if (bUpdateAngular)
		orient_ += deltatime * 0.5f * (angVel_*orient_);
	orient_.NormaliseThis();
	if (!overrideQuat_)
		rot_ = QuatToMatrix(orient_);
	linMom_ += force_  * deltatime;
	angMom_ += torque_ * deltatime;

	// Reset force accumulator
	force_  = Vector3();
	torque_ = Vector3();

	// Update render state
	const Matrix4x4 rot(overrideQuat_ ? rot_ : QuatToMatrix(orient_));
	renderContext_.transform = Matrix4x4::MakeTranslate(pos_) * rot * Matrix4x4::MakeUniScale(renderContext_.scale);
	{
		renderContext_.bsphere = bbk::TransformBSphere(renderContext_.transform, renderContext_.model->GetBSphere());
		const Vector3 scaledOffset(renderContext_.model->GetBSphereOffset() * renderContext_.scale);
		renderContext_.bsphere.center += rot * scaledOffset;
		renderContext_.bsphere.radius *= std::fabs(renderContext_.scale);
	}
	{
		renderContext_.aabb = bbk::TransformAABB(renderContext_.transform , renderContext_.model->GetAABB());
		const Vector3 scaledOffset(renderContext_.model->GetAABBOffset() * renderContext_.scale);
		renderContext_.aabb.center += rot * scaledOffset;
	}
	{
		renderContext_.obb = bbk::TransformOBB(renderContext_.transform, renderContext_.model->GetOBB());
		const Vector3 scaledOffset(renderContext_.model->GetOBBOffset() * renderContext_.scale);
		renderContext_.obb.center += rot * scaledOffset;
		renderContext_.obb.halfExtents *= renderContext_.scale;
	}
}

void BObject::Draw()
{
	gfx::DrawObject(renderContext_);
}

void BObject::SetGeometry(Model* model)
{
	renderContext_.model = model;
	invMass_ = 1.0f / (model->GetMass() * renderContext_.scale);
	invBodyInertiaTensor_ = Matrix3x3::MakeInverse(renderContext_.scale * model->GetInertiaTensor());
}

const Vector3& BObject::GetPosition() const
{
	return pos_;
}

void BObject::SetPosition(const Vector3& pos)
{
	pos_ = pos;
}

const Matrix3x3& BObject::GetRotation() const
{
	return rot_;
}

void BObject::SetRotation(const Matrix3x3& rot)
{
	rot_ = rot;
	overrideQuat_ = true;
}

void BObject::SetAngularVel(const Vector3& axisangle)
{
	angVel_ = Quat::MakeRotation(axisangle);
}

void BObject::AddAngularVel(const Vector3& axisangle)
{
	angVel_ += Quat::MakeRotation(axisangle);
}

void BObject::SetAngularMom(const Vector3& axisangle)
{
	angMom_ = axisangle;
}

void BObject::AddForce(const Vector3& force, const Vector3& contactPt)
{
	force_ += force;
	torque_ += (contactPt - pos_).Cross(force);
}

void BObject::ApplyNettForce(const Vector3& nettForce, const Vector3& contactPt)
{
	force_ = nettForce;
	torque_ = (contactPt - pos_).Cross(nettForce);
}

float BObject::GetMass() const
{
	return 1.0f / invMass_;
}

void BObject::SetInertiaTensor(const Matrix3x3& inertiaTensor)
{
	invBodyInertiaTensor_ = Matrix3x3::MakeInverse(inertiaTensor);
}

void BObject::Reset()
{
}

void BObject::SetScale(float scale)
{
	renderContext_.scale = scale;
	if (renderContext_.model)
	{
		invMass_ = 1.0f / (renderContext_.model->GetMass() * scale);
		invBodyInertiaTensor_ = Matrix3x3::MakeInverse(renderContext_.scale * renderContext_.model->GetInertiaTensor());
	}
}

void BObject::Read(xmlElement* rootnode)
{
	xmlElement *node = rootnode->GetChildElem("BObject");
	{
		xmlElement *posnode = node->GetChildElem("Position");
		pos_.x = posnode->GetAttrib("x")->GetValue_float();
		pos_.y = posnode->GetAttrib("y")->GetValue_float();
		pos_.z = posnode->GetAttrib("z")->GetValue_float();
	}
}

void BObject::Write(xmlElement* rootnode)
{
	xmlElement *node = new xmlElement("BObject");
	rootnode->AddChildElemToBack(node);
	{
		xmlElement *posnode = new xmlElement("Position");
		node->AddChildElemToBack(posnode);
		posnode->AddAttrib(xmlAttrib("x", pos_.x));
		posnode->AddAttrib(xmlAttrib("y", pos_.y));
		posnode->AddAttrib(xmlAttrib("z", pos_.z));
	}
}
} // namespace bbk
