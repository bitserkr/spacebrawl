#ifndef _MATRIX3X3_H
#define _MATRIX3X3_H

#include "vector3.h"

namespace bbk
{
class Matrix3x3
{
public:
	/// Default (element-wise) ctor; set to identity
	Matrix3x3(
		float x0=1.f, float x1=0.f, float x2=0.f,
		float y0=0.f, float y1=1.f, float y2=0.f,
		float z0=0.f, float z1=0.f, float z2=1.f);
	/// Array-of-floats ctor
	Matrix3x3(float* pfArray);
	/// Column Vector3 ctor
	Matrix3x3(const Vector3 &firstCol, const Vector3 &secondCol, const Vector3 &thirdCol);
	/// Copy ctor
	Matrix3x3(const Matrix3x3 &rhs);

	/**
	 * \name
	 * Overloaded operators
	 *///\{
	Matrix3x3& operator= (const Matrix3x3& rhs);
	Matrix3x3  operator+ (const Matrix3x3& rhs) const;
	Matrix3x3& operator+=(const Matrix3x3& rhs);
	Matrix3x3  operator- (const Matrix3x3& rhs) const;
	Matrix3x3& operator-=(const Matrix3x3& rhs);
	Matrix3x3  operator* (const Matrix3x3& rhs) const;
	Matrix3x3& operator*=(const Matrix3x3& rhs);
	Vector3    operator* (const Vector3 &vec)   const;
	//Matrix3x3  operator* (float f)              const;
	friend Matrix3x3 operator*(float f, const Matrix3x3& m);
	friend Matrix3x3 operator*(const Matrix3x3&m, float f) {return f*m;}
	//\}

	/**
	 * \name
	 * Row/column extractors
	 *///\{
	Vector3 GetRow(int rowIndex) const;
	Vector3 GetCol(int colIndex) const;
	//\}

	/**
	 * \name
	 * Instance auxiliary functions
	 *///\{
	void  SetIdentity();
	void  SetTranspose();
	void Normalise();

	float Determinant() const;
	//\}

	/**
	 * Static auxiliary functions
	 *///\{
	static Matrix3x3 MakeInverse(const Matrix3x3 &mtx);
	static Matrix3x3 MakeTranspose(const Matrix3x3 &mtx);
	static Matrix3x3 MakeScale(float x_scale, float y_scale, float z_scale);
	static Matrix3x3 MakeScale(const Vector3 &scale);
	static Matrix3x3 MakeUniScale(float scale);
	static Matrix3x3 MakeRotate(float axis_x, float axis_y, float axis_z, float angle_rad);
	static Matrix3x3 MakeRotate(const Vector3 &axis, float angle_rad);
	static Matrix3x3 MakeSkewSymmetric(const Vector3 &vec);
	//\}

	static const Matrix3x3 ZERO;
	static const Matrix3x3 IDENTITY;

	union
	{
		float elements[9];
		float m[3][3];
	};
}; // class Matrix3x3
} // namespace bbk

#endif /* _MATRIX3X3_H */
