#include <cstring>
#include "matrix4x4.h"

namespace bbk
{
const Matrix4x4 Matrix4x4::ZERO(
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f);

const Matrix4x4 Matrix4x4::IDENTITY(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Matrix4x4::Matrix4x4(
	float x0, float x1, float x2, float x3,
	float y0, float y1, float y2, float y3,
	float z0, float z1, float z2, float z3,
	float w0, float w1, float w2, float w3)
{
	elements[0]  = x0; elements[1]  = x1; elements[2]  = x2; elements[3]  = x3;
	elements[4]  = y0; elements[5]  = y1; elements[6]  = y2; elements[7]  = y3;
	elements[8]  = z0; elements[9]  = z1; elements[10] = z2; elements[11] = z3;
	elements[12] = w0; elements[13] = w1; elements[14] = w2; elements[15] = w3;
}

Matrix4x4::Matrix4x4(float * const pfArray)
{
	std::memcpy(elements, pfArray, sizeof(elements));
}

Matrix4x4::Matrix4x4(const Vector4 &col0, const Vector4 &col1, const Vector4 &col2, const Vector4 &col3)
{
	elements[0]  = col0.x; elements[1]  = col0.y; elements[2]  = col0.z; elements[3]  = col0.w;
	elements[4]  = col1.x; elements[5]  = col1.y; elements[6]  = col1.z; elements[7]  = col1.w;
	elements[8]  = col2.x; elements[9]  = col2.y; elements[10] = col2.z; elements[11] = col2.w;
	elements[12] = col3.x; elements[13] = col3.y; elements[14] = col3.z; elements[15] = col3.w;
}

Matrix4x4::Matrix4x4(const Matrix4x4 &rhs)
{
	std::memcpy(elements, rhs.elements, sizeof(elements));
}

Matrix4x4::Matrix4x4(const Matrix3x3 &rhs)
{
	elements[0]  = rhs.elements[0]; elements[1]  = rhs.elements[1]; elements[2]  = rhs.elements[2]; elements[3]  = 0.0f;
	elements[4]  = rhs.elements[3]; elements[5]  = rhs.elements[4]; elements[6]  = rhs.elements[5]; elements[7]  = 0.0f;
	elements[8]  = rhs.elements[6]; elements[9]  = rhs.elements[7]; elements[10] = rhs.elements[8]; elements[11] = 0.0f;
	elements[12] = 0.0f; elements[13] = 0.0f; elements[14] = 0.0f; elements[15] = 1.0f;
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4 &rhs)
{
	if (this != &rhs)
		std::memcpy(elements, rhs.elements, sizeof(elements));
	return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &rhs) const
{
	return Matrix4x4(
		elements[0] * rhs.elements[0]  + elements[4] * rhs.elements[1] + elements[8]  * rhs.elements[2] + elements[12] * rhs.elements[3],
		elements[1] * rhs.elements[0]  + elements[5] * rhs.elements[1] + elements[9]  * rhs.elements[2] + elements[13] * rhs.elements[3],
		elements[2] * rhs.elements[0]  + elements[6] * rhs.elements[1] + elements[10] * rhs.elements[2] + elements[14] * rhs.elements[3],
		elements[3] * rhs.elements[0]  + elements[7] * rhs.elements[1] + elements[11] * rhs.elements[2] + elements[15] * rhs.elements[3],
		
		elements[0] * rhs.elements[4]  + elements[4] * rhs.elements[5] + elements[8]  * rhs.elements[6] + elements[12] * rhs.elements[7],
		elements[1] * rhs.elements[4]  + elements[5] * rhs.elements[5] + elements[9]  * rhs.elements[6] + elements[13] * rhs.elements[7],
		elements[2] * rhs.elements[4]  + elements[6] * rhs.elements[5] + elements[10] * rhs.elements[6] + elements[14] * rhs.elements[7],
		elements[3] * rhs.elements[4]  + elements[7] * rhs.elements[5] + elements[11] * rhs.elements[6] + elements[15] * rhs.elements[7],
		
		elements[0] * rhs.elements[8]  + elements[4] * rhs.elements[9] + elements[8]  * rhs.elements[10] + elements[12] * rhs.elements[11],
		elements[1] * rhs.elements[8]  + elements[5] * rhs.elements[9] + elements[9]  * rhs.elements[10] + elements[13] * rhs.elements[11],
		elements[2] * rhs.elements[8]  + elements[6] * rhs.elements[9] + elements[10] * rhs.elements[10] + elements[14] * rhs.elements[11],
		elements[3] * rhs.elements[8]  + elements[7] * rhs.elements[9] + elements[11] * rhs.elements[10] + elements[15] * rhs.elements[11],
		
		elements[0] * rhs.elements[12] + elements[4] * rhs.elements[13] + elements[8]  * rhs.elements[14] + elements[12] * rhs.elements[15],
		elements[1] * rhs.elements[12] + elements[5] * rhs.elements[13] + elements[9]  * rhs.elements[14] + elements[13] * rhs.elements[15],
		elements[2] * rhs.elements[12] + elements[6] * rhs.elements[13] + elements[10] * rhs.elements[14] + elements[14] * rhs.elements[15],
		elements[3] * rhs.elements[12] + elements[7] * rhs.elements[13] + elements[11] * rhs.elements[14] + elements[15] * rhs.elements[15]);
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4 &rhs)
{
	*this = this->operator*(rhs);
	return *this;
}

Vector3 operator*(const Matrix4x4 &mtx, const Vector3 &vec)
{
	return Vector3(
		mtx.elements[0]*vec.x + mtx.elements[4]*vec.y + mtx.elements[8]*vec.z,
		mtx.elements[1]*vec.x + mtx.elements[5]*vec.y + mtx.elements[9]*vec.z,
		mtx.elements[2]*vec.x + mtx.elements[6]*vec.y + mtx.elements[10]*vec.z);
}

Vector4 operator*(const Matrix4x4 &mtx, const Vector4 &vec)
{
	return Vector4(
		mtx.elements[0]*vec.x + mtx.elements[4]*vec.y + mtx.elements[8] *vec.z + mtx.elements[12]*vec.w,
		mtx.elements[1]*vec.x + mtx.elements[5]*vec.y + mtx.elements[9] *vec.z + mtx.elements[13]*vec.w,
		mtx.elements[2]*vec.x + mtx.elements[6]*vec.y + mtx.elements[10]*vec.z + mtx.elements[14]*vec.w,
		mtx.elements[3]*vec.x + mtx.elements[7]*vec.y + mtx.elements[11]*vec.z + mtx.elements[15]*vec.w);
}

//Matrix4x4 Matrix4x4::operator-(const Matrix4x4 &rhs) const
//{
//	return Matrix4x4(
//		elements[0] - rhs.elements[0],
//		elements[1] - rhs.elements[1],
//		elements[2] - rhs.elements[2],
//		elements[3] - rhs.elements[3],
//		elements[4] - rhs.elements[4],
//		elements[5] - rhs.elements[5],
//		elements[6] - rhs.elements[6],
//		elements[7] - rhs.elements[7],
//		elements[8] - rhs.elements[8],
//		elements[9] - rhs.elements[9],
//		elements[10] - rhs.elements[10],
//		elements[11] - rhs.elements[11],
//		elements[12] - rhs.elements[12],
//		elements[13] - rhs.elements[13],
//		elements[14] - rhs.elements[14],
//		elements[15] - rhs.elements[15]);
//}

Vector4 Matrix4x4::GetRow(int rowIndex) const
{
	return Vector4(
		elements[rowIndex],
		elements[rowIndex + 4],
		elements[rowIndex + 8],
		elements[rowIndex + 12]);
}

Vector4 Matrix4x4::GetCol(int colIndex) const
{
	return Vector4(
		elements[colIndex * 4],
		elements[colIndex * 4 + 1],
		elements[colIndex * 4 + 2],
		elements[colIndex * 4 + 3]);
}
	
void Matrix4x4::SetIdentity()
{
	elements[0]  = 1.0f; elements[1]  = 0.0f; elements[2]  = 0.0f; elements[3]  = 0.0f;
	elements[4]  = 0.0f; elements[5]  = 1.0f; elements[6]  = 0.0f; elements[7]  = 0.0f;
	elements[8]  = 0.0f; elements[9]  = 0.0f; elements[10] = 1.0f; elements[11] = 0.0f;
	elements[12] = 0.0f; elements[13] = 0.0f; elements[14] = 0.0f; elements[15] = 1.0f;
}

void Matrix4x4::SetTranspose()
{
	float temp = elements[1];
	elements[1] = elements[4];
	elements[4] = temp;

	temp = elements[2];
	elements[2] = elements[8];
	elements[8] = temp;

	temp = elements[3];
	elements[3] = elements[12];
	elements[12] = temp;

	temp = elements[6];
	elements[6] = elements[9];
	elements[9] = temp;

	temp = elements[7];
	elements[7] = elements[13];
	elements[13] = temp;

	temp = elements[11];
	elements[11] = elements[14];
	elements[14] = temp;
}

void Matrix4x4::InvertThis()
{
	*this = this->Inverse();
}

inline float SubDet(const Matrix4x4 &mtx, int rowInd0, int rowInd1, int rowInd2, int colInd0, int colInd1, int colInd2)
{
	return mtx.m[rowInd0][colInd0] * (mtx.m[rowInd1][colInd1] * mtx.m[rowInd2][colInd2] - mtx.m[rowInd2][colInd1] * mtx.m[rowInd1][colInd2]) -
            mtx.m[rowInd0][colInd1] * (mtx.m[rowInd1][colInd0] * mtx.m[rowInd2][colInd2] - mtx.m[rowInd2][colInd0] * mtx.m[rowInd1][colInd2]) +
            mtx.m[rowInd0][colInd2] * (mtx.m[rowInd1][colInd0] * mtx.m[rowInd2][colInd1] - mtx.m[rowInd2][colInd0] * mtx.m[rowInd1][colInd1]);
}

float Matrix4x4::Determinant() const
{
	 return m[0][0] * SubDet(*this, 1, 2, 3, 1, 2, 3) -
            m[0][1] * SubDet(*this, 1, 2, 3, 0, 2, 3) +
            m[0][2] * SubDet(*this, 1, 2, 3, 0, 1, 3) -
            m[0][3] * SubDet(*this, 1, 2, 3, 0, 1, 2);
}

Matrix4x4 Matrix4x4::Transpose() const
{
	return Matrix4x4(
		elements[0], elements[4], elements[8], elements[12],
		elements[1], elements[5], elements[9], elements[13],
		elements[2], elements[6], elements[10], elements[14],
		elements[3], elements[7], elements[11], elements[15]);
}

Matrix4x4 Matrix4x4::Inverse() const
{
	float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
    float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    float invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;

    float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return Matrix4x4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

Matrix4x4 Matrix4x4::MakeInverse(const Matrix4x4 &mtx)
{
	return mtx.Inverse();
}

Matrix4x4 Matrix4x4::MakeTranspose(const Matrix4x4 &mtx)
{
	return mtx.Transpose();
}

Matrix4x4 Matrix4x4::MakeScale(float x_scale, float y_scale, float z_scale)
{
	return Matrix4x4(
		x_scale, 0.0f, 0.0f, 0.0f,
		0.0f, y_scale, 0.0f, 0.0f,
		0.0f, 0.0f, z_scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::MakeScale(const Vector3 &scale)
{
	return MakeScale(scale.x, scale.y, scale.z);
}

Matrix4x4 Matrix4x4::MakeUniScale(float scale)
{
	return MakeScale(scale, scale, scale);
}

Matrix4x4 Matrix4x4::MakeRotate(float axis_x, float axis_y, float axis_z, float angle_rad)
{
	Vector3 axis(axis_x, axis_y, axis_z);
	axis.NormaliseThis();

	float cosine = std::cos(angle_rad);
	float sine = std::sin(angle_rad);
	float coeff = 1.0f - cosine;

	return Matrix4x4(
		axis.x * axis.x + cosine * (1.0f - axis.x * axis.x),
		axis.x * axis.y * coeff + axis.z * sine,
		axis.x * axis.z * coeff - axis.y * sine,
		0.0f,

		axis.x * axis.y * coeff - axis.z * sine,
		axis.y * axis.y + cosine * (1.0f - axis.y * axis.y),
		axis.y * axis.z * coeff + axis.x * sine,
		0.0f,

		axis.x * axis.z * coeff + axis.y * sine,
		axis.y * axis.z * coeff - axis.x * sine,
		axis.z * axis.z + cosine * (1.0f - axis.z * axis.z),
		0.0f,

		0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::MakeRotate(const Vector3 &axis, float angle_rad)
{
	return MakeRotate(axis.x, axis.y, axis.z, angle_rad);
}

Matrix4x4 Matrix4x4::MakeTranslate(float disp_x, float disp_y, float disp_z)
{
	return Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		disp_x, disp_y, disp_z, 1.0f);
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vector3 &disp)
{
	return MakeTranslate(disp.x, disp.y, disp.z);
}
} // namespace bbk
