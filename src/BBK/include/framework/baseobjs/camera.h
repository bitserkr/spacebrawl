#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/vector3.h"
#include "math/matrix4x4.h"
#include "math/frustum.h"

namespace bbk
{
class Camera
{
public:
	Camera();
	virtual ~Camera();

	/** @name
	 *  Methods for camera vectors *///\{
	const Vector3& GetPosition()    const {return pos_;}
	const Vector3& GetViewVector()  const {return viewVec_;}
	const Vector3& GetRightVector() const {return rightVec_;}
	const Vector3& GetGlobalUpVec() const {return globUp_;}

	void SetPosition(const Vector3 &pos)        {pos_     = pos; isDirty_ = true;}
	void SetViewVector(const Vector3 &viewVec)  {viewVec_ = viewVec; UpdateSphericalFromVec(); isDirty_ = true;}
	void SetTargetPos(const Vector3 &targetPos) {viewVec_ = targetPos - pos_; UpdateSphericalFromVec(); isDirty_ = true;}
	void SetGlobalUpVec(const Vector3 &globUp)  {globUp_  = globUp;}
	//\}

	/** @name
	 *  Methods for spherical coordinates *///\{
	float GetAzimuth()     const {return azimuth_;}
	float GetInclination() const {return inclin_;}
	float GetRadius()      const {return radius_;}

	void SetAzimuth(float azimuth_rad) {azimuth_ = azimuth_rad; UpdateVecFromSpherical(); isDirty_ = true;}
	void SetInclin(float inclin_rad)   {inclin_  = inclin_rad;  UpdateVecFromSpherical(); isDirty_ = true;}
	void SetRadius(float radius_rad)   {radius_  = radius_rad;  UpdateVecFromSpherical(); isDirty_ = true;}
	//\}

	/** @name
	 *  Directional manipulation *///\{
	/// Rotate about x-axis
	void Pitch(float angle_rad);
	/// Rotate about y-axis
	void Yaw(float angle_rad);
	/// Rotate about z-axis
	void Roll(float angle_rad);
	//\}

	/** @name
	 *  Positional manipulation *///\{
	/// Vector displacement
	void Displace(const Vector3 &disp) {pos_ += disp; isDirty_ = true;}
	/// Spherical coords displacement without updating view vector
	void OrbitalDisp(const Vector3 &pivot, float dAzimuth_rad, float dInclin_rad);
	/// Spherical coords displacement and look at pivot
	void OrbitalDispAndLook(const Vector3 &pivot, float dAzimuth_rad, float dInclin_rad);
	//\}

	/** @name
	 *  Camera matrices accessors *///\{
	const Matrix4x4& GetProjectionMtx()  const {return mtx44_proj_;}
	const Matrix4x4& GetWorldToViewMtx() const {if (isDirty_) UpdatePlaneEqns(); return mtx44_worldToView_;}
	const Matrix4x4& GetViewToWorldMtx() const {if (isDirty_) UpdatePlaneEqns(); return mtx44_viewToWorld_;}
	//\}

	/** @name
	 *  Frustum *///\{
	const Frustum& GetFrustum() const {if (isDirty_) UpdatePlaneEqns(); return frustum_;}
	void DrawFrustum() const;
	/// Order in array is l, r, b, t, n, f
	void GetPlaneDists(float planeDists[6]) const;
	float GetNearPlaneDist() const {return n_;}
	float GetFarPlaneDist()  const {return f_;}
	
	void SetClipPlanes(float n, float f) {n_ = n; f_ = f;}

	float GetVerticalFoV() const {return vertFoVDeg_;}
	float GetAspectRatio() const {return aspectRatio_;}
	//\}

protected:
	/** @name
	 *  Camera vectors *///\{
	Vector3 pos_;
	Vector3 viewVec_;
	mutable Vector3 rightVec_;
	Vector3 globUp_;
	float   rollAngle_;
	//\}

	/** @name
	 *  Spherical coordinates *///\{
	float azimuth_; ///< Angle of horizontal sweep
	float inclin_;  ///< Angle of rise/drop from horizontal plane to view vector
	float radius_;
	//\}
	
	Matrix4x4 mtx44_proj_;
	mutable Matrix4x4 mtx44_worldToView_;
	mutable Matrix4x4 mtx44_viewToWorld_;

	/** @name
	 *  Frustum *///\{
	float l_, r_, b_, t_, n_, f_;
	float vertFoVDeg_;
	float aspectRatio_;
	mutable Frustum frustum_;
	//\}

	mutable bool isDirty_;

	/** @name
	 *  Private helper functions *///\{
	        void UpdateVecFromSpherical();
	        void UpdateSphericalFromVec();
	virtual void ComputeProjMtx() = 0;
	        void ComputeViewMtx() const;
			void UpdatePlaneEqns() const;
	//\}
}; // class Camera
} // namespace bbk

#endif /* _CAMERA_H */
