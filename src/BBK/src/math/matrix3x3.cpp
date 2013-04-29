#include <cstring>
#include "matrix3x3.h"

namespace bbk
{
const Matrix3x3 Matrix3x3::ZERO(
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f);

const Matrix3x3 Matrix3x3::IDENTITY(
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f);

Matrix3x3::Matrix3x3(
	float x0, float x1, float x2,
	float y0, float y1, float y2,
	float z0, float z1, float z2)
{
	elements[0] = x0; elements[1] = x1; elements[2] = x2;
	elements[3] = y0; elements[4] = y1; elements[5] = y2;
	elements[6] = z0; elements[7] = z1; elements[8] = z2;
}

Matrix3x3::Matrix3x3(float * const pfArray)
{
	std::memcpy(elements, pfArray, sizeof(elements));
}

Matrix3x3::Matrix3x3(const Vector3 &col0, const Vector3 &col1, const Vector3 &col2)
{
	elements[0] = col0.x; elements[1] = col0.y; elements[2] = col0.z;
	elements[3] = col1.x; elements[4] = col1.y; elements[5] = col1.z;
	elements[6] = col2.x; elements[7] = col2.y; elements[8] = col2.z;
}

Matrix3x3::Matrix3x3(const Matrix3x3 &rhs)
{
	std::memcpy(elements, rhs.elements, sizeof(elements));
}

Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs)
{
	if (this != &rhs)
		std::memcpy(elements, rhs.elements, sizeof(elements));
	return *this;
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3& rhs) const
{
	return Matrix3x3(
		elements[0] + rhs.elements[0], elements[1] + rhs.elements[1], elements[2] + rhs.elements[2],
		elements[3] + rhs.elements[3], elements[4] + rhs.elements[4], elements[5] + rhs.elements[5],
		elements[6] + rhs.elements[6], elements[7] + rhs.elements[7], elements[8] + rhs.elements[8]);
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& rhs)
{
	*this = this->operator+(rhs);
	return *this;
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3& rhs) const
{
	return Matrix3x3(
		elements[0] - rhs.elements[0], elements[1] - rhs.elements[1], elements[2] - rhs.elements[2],
		elements[3] - rhs.elements[3], elements[4] - rhs.elements[4], elements[5] - rhs.elements[5],
		elements[6] - rhs.elements[6], elements[7] - rhs.elements[7], elements[8] - rhs.elements[8]);
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& rhs)
{
	*this = this->operator-(rhs);
	return *this;
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &rhs) const
{
	return Matrix3x3(
		elements[0] * rhs.elements[0] + elements[3] * rhs.elements[1] + elements[6] * rhs.elements[2],
		elements[1] * rhs.elements[0] + elements[4] * rhs.elements[1] + elements[7] * rhs.elements[2],
		elements[2] * rhs.elements[0] + elements[5] * rhs.elements[1] + elements[8] * rhs.elements[2],
		
		elements[0] * rhs.elements[3] + elements[3] * rhs.elements[4] + elements[6] * rhs.elements[5],
		elements[1] * rhs.elements[3] + elements[4] * rhs.elements[4] + elements[7] * rhs.elements[5],
		elements[2] * rhs.elements[3] + elements[5] * rhs.elements[4] + elements[8] * rhs.elements[5],
		
		elements[0] * rhs.elements[6] + elements[3] * rhs.elements[7] + elements[6] * rhs.elements[8],
		elements[1] * rhs.elements[6] + elements[4] * rhs.elements[7] + elements[7] * rhs.elements[8],
		elements[2] * rhs.elements[6] + elements[5] * rhs.elements[7] + elements[8] * rhs.elements[8]);
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3 &rhs)
{
	*this = this->operator*(rhs);
	return *this;
}

Vector3 Matrix3x3::operator*(const Vector3 &vec) const
{
	return Vector3(
		elements[0] * vec.x + elements[3] * vec.y + elements[6] * vec.z,
		elements[1] * vec.x + elements[4] * vec.y + elements[7] * vec.z,
		elements[2] * vec.x + elements[5] * vec.y + elements[8] * vec.z);
}

//Matrix3x3 Matrix3x3::operator*(float f) const
//{
//	return Matrix3x3(
//		f * elements[0], f * elements[1], f * elements[2],
//		f * elements[3], f * elements[4], f * elements[5],
//		f * elements[6], f * elements[7], f * elements[8]);
//}

Matrix3x3 operator*(float f, const Matrix3x3& m)
{
	return Matrix3x3(
		f * m.elements[0], f * m.elements[1], f * m.elements[2],
		f * m.elements[3], f * m.elements[4], f * m.elements[5],
		f * m.elements[6], f * m.elements[7], f * m.elements[8]);
}

Vector3 Matrix3x3::GetRow(int rowIndex) const
{
	return Vector3(
		elements[rowIndex    ],
		elements[rowIndex + 3],
		elements[rowIndex + 6]);
}

Vector3 Matrix3x3::GetCol(int colIndex) const
{
	return Vector3(
		elements[3 * colIndex    ],
		elements[3 * colIndex + 1],
		elements[3 * colIndex + 2]);
}

	
void Matrix3x3::SetIdentity()
{
	elements[0] = 1.0f; elements[1] = 0.0f; elements[2] = 0.0f;
	elements[3] = 0.0f; elements[4] = 1.0f; elements[5] = 0.0f;
	elements[6] = 0.0f; elements[7] = 0.0f; elements[8] = 1.0f;
}

void Matrix3x3::SetTranspose()
{
	float temp = elements[1];
	elements[1] = elements[3];
	elements[3] = temp;

	temp = elements[2];
	elements[2] = elements[6];
	elements[6] = temp;

	temp = elements[5];
	elements[5] = elements[7];
	elements[7] = temp;
}

void Matrix3x3::Normalise()
{
	Vector3 u(elements[0], elements[1], elements[2]);
	Vector3 v(elements[3], elements[4], elements[5]);
	Vector3 w(elements[6], elements[7], elements[8]);

	u.NormaliseThis();
	v.NormaliseThis();
	w.NormaliseThis();

	elements[0] = u.x;
	elements[1] = u.y;
	elements[2] = u.z;
	elements[3] = v.x;
	elements[4] = v.y;
	elements[5] = v.z;
	elements[6] = w.x;
	elements[7] = w.y;
	elements[8] = w.z;
}

float Matrix3x3::Determinant() const
{
	return (
		  elements[0] * (elements[4] * elements[8] - elements[5] * elements[7])
		+ elements[3] * (elements[7] * elements[2] - elements[8] * elements[1])
		+ elements[6] * (elements[1] * elements[5] - elements[2] * elements[4]));
}

//Matrix3x3 Matrix3x3::MakeInverse(const Matrix3x3 &mtx)
//{
//	Matrix3x3 inv(
//		mtx.elements[4]*mtx.elements[8] - mtx.elements[5]*mtx.elements[7],
//		mtx.elements[2]*mtx.elements[7] - mtx.elements[1]*mtx.elements[8],
//		mtx.elements[1]*mtx.elements[5] - mtx.elements[2]*mtx.elements[4],
//		mtx.elements[5]*mtx.elements[6] - mtx.elements[3]*mtx.elements[8],
//		mtx.elements[0]*mtx.elements[8] - mtx.elements[2]*mtx.elements[6],
//		mtx.elements[2]*mtx.elements[3] - mtx.elements[0]*mtx.elements[5],
//		mtx.elements[3]*mtx.elements[7] - mtx.elements[4]*mtx.elements[6],
//		mtx.elements[1]*mtx.elements[6] - mtx.elements[0]*mtx.elements[7],
//		mtx.elements[0]*mtx.elements[4] - mtx.elements[1]*mtx.elements[3]);
//
//	float det = mtx.elements[0]*inv.elements[0] + mtx.elements[1]*inv.elements[3] + mtx.elements[2]*inv.elements[6];
//	if (std::fabs(det) > 0.000001f)
//	{
//		float invdet = 1.0f / det;
//		inv = inv * invdet;
//		return inv;
//	}
//	return Matrix3x3::ZERO;
//}

Matrix3x3 Matrix3x3::MakeInverse(const Matrix3x3 &mtx)
{
	float coeff = 1.0f / mtx.Determinant();

	return Matrix3x3(
		coeff * (mtx.elements[4] * mtx.elements[8] - mtx.elements[5] * mtx.elements[7]),
		coeff * (mtx.elements[6] * mtx.elements[5] - mtx.elements[3] * mtx.elements[8]),
		coeff * (mtx.elements[3] * mtx.elements[7] - mtx.elements[6] * mtx.elements[4]),

		coeff * (mtx.elements[7] * mtx.elements[2] - mtx.elements[1] * mtx.elements[8]),
		coeff * (mtx.elements[0] * mtx.elements[8] - mtx.elements[2] * mtx.elements[6]),
		coeff * (mtx.elements[1] * mtx.elements[6] - mtx.elements[0] * mtx.elements[7]),

		coeff * (mtx.elements[1] * mtx.elements[5] - mtx.elements[2] * mtx.elements[4]),
		coeff * (mtx.elements[2] * mtx.elements[3] - mtx.elements[0] * mtx.elements[5]),
		coeff * (mtx.elements[0] * mtx.elements[4] - mtx.elements[1] * mtx.elements[3]));
}

Matrix3x3 Matrix3x3::MakeTranspose(const Matrix3x3 &mtx)
{
	return Matrix3x3(
		mtx.elements[0], mtx.elements[3], mtx.elements[6],
		mtx.elements[1], mtx.elements[4], mtx.elements[7],
		mtx.elements[2], mtx.elements[5], mtx.elements[8]);
}

Matrix3x3 Matrix3x3::MakeScale(float x_scale, float y_scale, float z_scale)
{
	return Matrix3x3(
		x_scale, 0.0f, 0.0f,
		0.0f, y_scale, 0.0f,
		0.0f, 0.0f, z_scale);
}

Matrix3x3 Matrix3x3::MakeScale(const Vector3 &scale)
{
	return MakeScale(scale.x, scale.y, scale.z);
}

Matrix3x3 Matrix3x3::MakeUniScale(float scale)
{
	return MakeScale(scale, scale, scale);
}

Matrix3x3 Matrix3x3::MakeRotate(float axis_x, float axis_y, float axis_z, float angle_rad)
{
	Vector3 axis(axis_x, axis_y, axis_z);
	axis.NormaliseThis();

	float cosine = std::cos(angle_rad);
	float sine = std::sin(angle_rad);
	float coeff = 1.0f - cosine;

	return Matrix3x3(
		axis.x * axis.x + cosine * (1.0f - axis.x * axis.x),
		axis.x * axis.y * coeff + axis.z * sine,
		axis.x * axis.z * coeff - axis.y * sine,

		axis.x * axis.y * coeff - axis.z * sine,
		axis.y * axis.y + cosine * (1.0f - axis.y * axis.y),
		axis.y * axis.z * coeff + axis.x * sine,

		axis.x * axis.z * coeff + axis.y * sine,
		axis.y * axis.z * coeff - axis.x * sine,
		axis.z * axis.z + cosine * (1.0f - axis.z * axis.z));
}

Matrix3x3 Matrix3x3::MakeRotate(const Vector3 &axis, float angle_rad)
{
	return MakeRotate(axis.x, axis.y, axis.z, angle_rad);
}

Matrix3x3 Matrix3x3::MakeSkewSymmetric(const Vector3 &vec)
{
	return Matrix3x3(
		0.0f,
		vec.z,
		-vec.y,

		-vec.z,
		0.0f,
		vec.x,

		vec.y,
		-vec.x,
		0.0f);
}
} // namespace bbk
