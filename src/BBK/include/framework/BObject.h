#ifndef _BOBJECT_H
#define _BOBJECT_H

#include "BArchive.h"
#include "graphics/rendercontext.h"
#include "math/quaternion.h"

namespace bbk
{
class BObject : public BArchive
{
public:
	BObject();
	
	void Integrate(float deltatime);
	void Update(float deltatime);
	void Draw();

	void SetGeometry(Model* model);

	/** @name
	 *  State vector methods *///\{
	const Vector3&   GetPosition() const;
	void             SetPosition(const Vector3& pos);
	const Quat&      GetQuat() const {return orient_;}
	const Matrix3x3& GetRotation() const;
	void             SetRotation(const Matrix3x3 &rot);
	const Vector3&   GetVelocity() const {return linVel_;}
	const Quat&      GetAngularVel() const {return angVel_;}
	void             SetAngularVel(const Vector3& axisangle);
	void             AddAngularVel(const Vector3& axisangle);
	const Vector3&   GetAngularMom() const {return angMom_;}
	void             SetAngularMom(const Vector3& axisangle);
	void             AddForce(const Vector3& force, const Vector3& contactPt=Vector3());
	void             ApplyNettForce(const Vector3& nettForce, const Vector3& contactPt=Vector3());
	float            GetMass() const;
	void             SetInertiaTensor(const Matrix3x3& inertiaTensor);
	void             Reset();
	//\}
	
	/** @name
	 *  Render state *///\{
	const Matrix4x4& GetTransform() const    {return renderContext_.transform;}
	float            GetScale()     const    {return renderContext_.scale;}
	void             SetScale(float scale);
	Model*           GetModel()     const    {return renderContext_.model;}
	void             SetModel(Model* pModel) {renderContext_.model = pModel;}
	const BSphere&   GetBSphere()   const    {return renderContext_.bsphere;}
	const AABB&      GetAABB()      const    {return renderContext_.aabb;}
	const OBB&       GetOBB()       const    {return renderContext_.obb;}
	RenderContext&   GetRenderContext()      {return renderContext_;}
	//\}

private:
	/** @name
	 *  State vector *///\{
	Vector3   pos_;
	Matrix3x3 rot_;
	Quat      orient_;
	Vector3   linMom_;
	Vector3   angMom_;

	Vector3   linVel_;
	Quat      angVel_;
	Vector3   force_;
	Vector3   torque_;
	
	float     invMass_;
	Matrix3x3 invBodyInertiaTensor_;
	//\}

	bool overrideQuat_;

	RenderContext renderContext_;

	virtual void Read(xmlElement* rootnode);
	virtual void Write(xmlElement* rootnode);
}; // class BObject
} // namespace bbk

#endif /* _BOBJECT_H */
