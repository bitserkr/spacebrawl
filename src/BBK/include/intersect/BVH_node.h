#ifndef _BVH_NODE_H
#define _BVH_NODE_H

#include "OBB.h"

namespace bbk
{
struct BVHNode
{
	OBB       obb;
	size_t    numVerts;
	Vector3   *triVerts; ///< Always 3 vectors if any present
	BVHNode   *left, *right;
	
	BVHNode(const OBB& inOBB=OBB(), Vector3 *vertArray=nullptr) : obb(inOBB), triVerts(vertArray), left(nullptr), right(nullptr), numVerts(0) {}
	~BVHNode() {if (triVerts) delete[] triVerts; if (left) delete left; if (right) delete right;}
};
} // namespace bbk

#endif /* _BVH_NODE_H */
