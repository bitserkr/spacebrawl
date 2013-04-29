#ifndef _RENDERCONTEXT_H
#define _RENDERCONTEXT_H

#include "math/matrix4x4.h"
#include "intersect/intersect.h"

namespace bbk
{
class Model;

struct RenderContext
{
	Matrix4x4 transform;
	float     scale;
	Model*    model;
	BSphere   bsphere;
	AABB      aabb;
	OBB       obb;
	char      planeInd; ///< Cached plane index for plane-coherency culling

	RenderContext(): model(nullptr), scale(1.0f), planeInd(0) {}
}; // struct RenderContext
} // namespace bbk

#endif /* _RENDERCONTEXT_H */
