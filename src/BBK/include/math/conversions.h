#ifndef _CONVERSIONS_H
#define _CONVERSIONS_H

#include "quaternion.h"
#include "matrix3x3.h"

namespace bbk
{
inline Matrix3x3 QuatToMatrix(const Quat& q) {
	//const Quat q(inq.Normalise());
	const float xsq2 = 2.0f * q.v.x * q.v.x;
	const float ysq2 = 2.0f * q.v.y * q.v.y;
	const float zsq2 = 2.0f * q.v.z * q.v.z;
	const float xy2  = 2.0f * q.v.x * q.v.y;
	const float yz2  = 2.0f * q.v.y * q.v.z;
	const float xz2  = 2.0f * q.v.x * q.v.z;
	const float sx2  = 2.0f * q.s   * q.v.x;
	const float sy2  = 2.0f * q.s   * q.v.y;
	const float sz2  = 2.0f * q.s   * q.v.z;

	return Matrix3x3(
		1.0f - ysq2 - zsq2, xy2 + sz2, xz2 - sy2,
		xy2 - sz2, 1.0f - xsq2 - zsq2, yz2 + sx2,
		xz2 + sy2, yz2 - sx2, 1.0f - xsq2 - ysq2);
}
} // namespace bbk

#endif /* _CONVERSIONS_H */
