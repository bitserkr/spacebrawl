#include "vector4.h"
#include "vector3.h"

namespace bbk
{
Vector4::Vector4(const Vector3 &rhs, float warg) : x(rhs.x), y(rhs.y), z(rhs.z), w(warg) {}
} // namespace bbk
