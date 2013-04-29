#ifndef _MATRIX4X4_H
#define _MATRIX4X4_H

#include "vector3.h"
#include "vector4.h"
#include "matrix3x3.h"

namespace bbk
{
class Matrix4x4
{
public:
	/// Default (element-wise) ctor; set to identity
	Matrix4x4(
		float x0=1.f, float x1=0.f, float x2=0.f, float x3=0.f,
		float y0=0.f, float y1=1.f, float y2=0.f, float y3=0.f,
		float z0=0.f, float z1=0.f, float z2=1.f, float z3=0.f,
		float w0=0.f, float w1=0.f, float w2=0.f, float w3=1.f);
	/// Array-of-floats ctor
	Matrix4x4(float* const pfArray);
	/// Column Vector4 ctor
	Matrix4x4(const Vector4 &firstCol, const Vector4 &secondCol, const Vector4 &thirdCol, const Vector4 &fourthCol = Vector4(0.f,0.f,0.f,1.f));
	/// Copy ctor
	Matrix4x4(const Matrix4x4 &rhs);
	/// M3x3 to M4x4 ctor
	Matrix4x4(const Matrix3x3 &rhs);

	/**
	 * \name
	 * Overloaded operators
	 *///\{
	Matrix4x4& operator=(const Matrix4x4 &rhs);
	Matrix4x4  operator*(const Matrix4x4 &rhs) const;
	Matrix4x4& operator*=(const Matrix4x4 &rhs);
	friend Vector3 operator*(const Matrix4x4 &mtx, const Vector3 &vec);
	friend Vector4 operator*(const Matrix4x4 &mtx, const Vector4 &vec);

	//Matrix4x4 operator-(const Matrix4x4 &rhs) const;
	//\}

	/** @name
	 * Row/column extractors *///\{
	Vector4 GetRow(int rowIndex) const;
	Vector4 GetCol(int colIndex) const;
	//\}

	/**
	 * \name
	 * Instance auxiliary functions
	 *///\{
	void SetIdentity();
	void SetTranspose();
	void InvertThis();

	float     Determinant() const;
	Matrix4x4 Transpose()   const;
	Matrix4x4 Inverse()     const;
	//\}

	/**
	 * Static auxiliary functions
	 *///\{
	static Matrix4x4 MakeInverse(const Matrix4x4 &mtx);
	static Matrix4x4 MakeTranspose(const Matrix4x4 &mtx);
	static Matrix4x4 MakeScale(float x_scale, float y_scale, float z_scale);
	static Matrix4x4 MakeScale(const Vector3 &scale);
	static Matrix4x4 MakeUniScale(float scale);
	static Matrix4x4 MakeRotate(float axis_x, float axis_y, float axis_z, float angle_rad);
	static Matrix4x4 MakeRotate(const Vector3 &axis, float angle_rad);
	static Matrix4x4 MakeTranslate(float disp_x, float disp_y, float disp_z);
	static Matrix4x4 MakeTranslate(const Vector3 &disp);
	//\}

	static const Matrix4x4 ZERO;
	static const Matrix4x4 IDENTITY;

	union
	{
		float elements[16];
		float m[4][4];
	};
}; // class Matrix4x4
} // namespace bbk

#endif /* _MATRIX4X4_H */
