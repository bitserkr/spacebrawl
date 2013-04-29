#ifndef _VECTOR3_H
#define _VECTOR3_H

#include <cmath>

namespace bbk
{
struct Vector4;

typedef struct Vector3 Point3;
typedef struct Vector3 Vec3;
struct Vector3
{
	/// Default ctor
	Vector3(float x_arg=0.f, float y_arg=0.f, float z_arg=0.f) : x(x_arg), y(y_arg), z(z_arg) {}
	/// Copy ctor
	Vector3(const Vector3 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
	/// Vector4-to-Vector3 copy ctor
	Vector3(const Vector4 &rhs);

	/**
	 * \name
	 * Overloaded operators
	 *///\{
	Vector3& operator= (const Vector3 &rhs)       {x=rhs.x; y=rhs.y; z=rhs.z; return *this;}
	Vector3  operator+ (const Vector3 &rhs) const {return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);}
	Vector3  operator+ (      float    f  ) const {return Vector3(x+f, y+f, z+f);}
	Vector3& operator+=(const Vector3 &rhs)       {x+=rhs.x; y+=rhs.y; z+=rhs.z; return *this;}
	Vector3& operator+=(      float    f  )       {x+=f; y+=f; z+=f; return *this;}
	Vector3  operator- (const Vector3 &rhs) const {return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);}
	Vector3  operator- (const float    f  ) const {return Vector3(x-f, y-f, z-f);}
	Vector3& operator-=(const Vector3 &rhs)       {x-=rhs.x; y-=rhs.y; z-=rhs.z; return *this;}
	Vector3& operator-=(      float    f  )       {x-=f; y-=f; z-=f; return *this;}
	Vector3  operator* (      float    f  ) const {return Vector3(x*f, y*f, z*f);}
	Vector3& operator*=(      float    f  )       {x*=f; y*=f; z*=f; return *this;}
	Vector3  operator/ (      float    f  ) const {return Vector3(x/f, y/f, z/f);}
	Vector3& operator/=(      float    f  )       {x/=f; y/=f; z/=f; return *this;}
	Vector3  operator+()                    const {return Vector3(x, y, z);}
	Vector3  operator-()                    const {return Vector3(-x, -y, -z);}
	
	friend Vector3 operator* (float f, const Vector3& v) {return v * f;}
	//\}

	/** @name
	 *  Auxiliary functions *///\{
	float   MagnitudeSq()   const {return x*x + y*y + z*z;}
	float   Magnitude()     const {return std::sqrt(MagnitudeSq());}
	Vector3 Normalise()     const {float m = Magnitude(); if (m < 0.000001f) return *this; float invm = 1.0f / m; return Vector3(x*invm, y*invm, z*invm);}
	void    NormaliseThis()       {float m = Magnitude(); if (m < 0.000001f) return; float invm = 1.0f / m; this->operator*=(invm);}

	float   Dot(const Vector3 &rhs)   const {return x*rhs.x + y*rhs.y + z*rhs.z;}
	Vector3 Cross(const Vector3 &rhs) const {return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);}

	void    SetCoords(float xarg, float yarg, float zarg) {x=xarg; y=yarg; z=zarg;}
	//\}

	float x, y, z;
}; // struct Vector3
} // namespace bbk

#endif /* _VECTOR3_H */
