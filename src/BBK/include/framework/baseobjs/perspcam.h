#ifndef _PERSPCAM_H
#define _PERSPCAM_H

#include "camera.h"

namespace bbk
{
class PerspCam : public Camera
{
public:
	PerspCam();
	virtual ~PerspCam();

	void Init(float l, float r, float b, float t, float n, float f);
	void Init(float vertFoV_deg, float aspectRatio, float n, float f);

protected:
	virtual void ComputeProjMtx();
}; // class PerspCam
} // namespace bbk

#endif /* _PERSPCAM_H */
