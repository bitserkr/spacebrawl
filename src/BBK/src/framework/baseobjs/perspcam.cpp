#include "framework/baseobjs/perspcam.h"
#include "graphics/graphics.h"

namespace bbk
{
PerspCam::PerspCam()
{}

PerspCam::~PerspCam() {}

void PerspCam::Init(float l, float r, float b, float t, float n, float f)
{
	l_ = l; r_ = r; b_ = b; t_ = t; n_ = n; f_ = f;
	vertFoVDeg_ = bbk::RAD_TO_DEG(std::atan2(t, n) * 2.0f);
	aspectRatio_ = (r - l) / (t - b);
	ComputeProjMtx();
}

void PerspCam::Init(float fov, float a, float n, float f)
{
	float halfHeight = std::tan(DEG_TO_RAD(fov) * 0.5f) * n;
	float halfWidth  = a * halfHeight;
	vertFoVDeg_ = fov;
	aspectRatio_ = a;

	Init(-halfWidth, halfWidth, -halfHeight, halfHeight, n, f);
}

void PerspCam::ComputeProjMtx()
{
	mtx44_proj_ = gfx::MakePerspProjMtx(l_, r_, b_, t_, n_, f_);
}
} // namespace bbk
