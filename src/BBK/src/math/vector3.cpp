#include "vector3.h"
#include "vector4.h"

namespace bbk
{
Vector3::Vector3(const Vector4 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
} // namespace bbk
