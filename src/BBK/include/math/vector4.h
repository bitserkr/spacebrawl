#ifndef _VECTOR4_H
#define _VECTOR4_H

#include <cmath>

namespace bbk
{
struct Vector3;

typedef struct Vector4 Point4;
typedef struct Vector4 Vec4;
struct Vector4
{
	/// Default ctor
	Vector4(float xarg=0.f, float yarg=0.f, float zarg=0.f, float warg=0.f) : x(xarg), y(yarg), z(zarg), w(warg) {}
	/// Copy ctor
	Vector4(const Vector4 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
	/// Vector3-to-Vector4 copy ctor
	Vector4(const Vector3 &rhs, float w = 0.0f);

	/**
	 * \name
	 * Overloaded operators
	 *///\{
	Vector4& operator= (const Vector4 &rhs)       {x=rhs.x; y=rhs.y; z=rhs.z; w=rhs.w; return *this;}
	Vector4  operator+ (const Vector4 &rhs) const {return Vector4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);}
	Vector4  operator+ (      float    f  ) const {return Vector4(x+f, y+f, z+f, w+f);}
	Vector4& operator+=(const Vector4 &rhs)       {x+=rhs.x; y+=rhs.y; z+=rhs.z; w+=rhs.w; return *this;}
	Vector4& operator+=(      float    f  )       {x+=f; y+=f; z+=f; w+=f; return *this;}
	Vector4  operator- (const Vector4 &rhs) const {return Vector4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);}
	Vector4  operator- (const float    f  ) const {return Vector4(x-f, y-f, z-f, w-f);}
	Vector4& operator-=(const Vector4 &rhs)       {x-=rhs.x; y-=rhs.y; z-=rhs.z; w-=rhs.w; return *this;}
	Vector4& operator-=(      float    f  )       {x-=f; y-=f; z-=f; w-=f; return *this;}
	Vector4  operator* (      float    f  ) const {return Vector4(x*f, y*f, z*f, w*f);}
	Vector4& operator*=(      float    f  )       {x*=f; y*=f; z*=f; w*=f; return *this;}
	Vector4  operator/ (      float    f  ) const {return Vector4(x/f, y/f, z/f, w/f);}
	Vector4& operator/=(      float    f  )       {x/=f; y/=f; z/=f; w/=f; return *this;}
	Vector4  operator+()                    const {return Vector4(x, y, z, w);}
	Vector4  operator-()                    const {return Vector4(-x, -y, -z, -w);}
	
	friend Vector4 operator* (float f, const Vector4& v) {return v * f;}
	//\}

	/** @name
	 *  Auxiliary functions *///\{
	float   MagnitudeSq()   const {return x*x + y*y + z*z + w*w;}
	float   Magnitude()     const {return std::sqrt(MagnitudeSq());}
	Vector4 Normalise()     const {float m = 1.0f / Magnitude(); return Vector4(x*m, y*m, z*m, w*m);}
	void    NormaliseThis()       {float m = 1.0f / Magnitude(); this->operator*=(m);}

	float   Dot(const Vector4 &rhs)   const {return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w;}
	Vector4 Cross(const Vector4 &rhs) const {return Vector4(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);}

	void    SetCoords(float xarg, float yarg, float zarg, float warg) {x=xarg; y=yarg; z=zarg; w=warg;}
	//\}

	float x, y, z, w;
}; // struct Vector4
} // namespace bbk

#endif /* _VECTOR4_H */
