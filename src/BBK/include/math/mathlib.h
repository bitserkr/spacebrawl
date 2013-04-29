#ifndef _MATHLIB_H
#define _MATHLIB_H

#include "tuple.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "frustum.h"
#include "lines.h"
#include "conversions.h"

namespace bbk
{
const float PIf      = 3.14159265f;
const float HALF_PIf = 0.5f * PIf;
const float TWO_PIf  = 2.0f * PIf;
const float INV_PIf  = 1.0f / PIf;
const float INV_180  = 1.0f / 180.0f;
const float EPSILONf = 1.0e-05f;

inline float DEG_TO_RAD(float deg) {return deg * PIf * INV_180;}
inline float RAD_TO_DEG(float rad) {return rad * INV_PIf * 180.0f;}

void ComputeRoots(const Matrix3x3& mtx, double roots[3]);
void EigenSolver(const Matrix3x3& mtx, float* eigenValues, Vector3* eigenVecs);
void GetEigenVecs(const Matrix3x3& mtx, Vector3* eigenVecs);

int JacobiSolver3(const Matrix3x3& covar, double eigenvals[3], double eigenvecs[3][3]);
} // namespace bbk

#endif /* _MATHLIB_H */
