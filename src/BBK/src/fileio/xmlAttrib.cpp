#include <cstdio>
#include "xmlAttrib.h"

namespace bbk
{
void xmlAttrib::SetValue(int int_value)
{
	char buffer[64] = {0};
	std::sprintf(buffer, "%d", int_value);
	value_ = buffer;
}

void xmlAttrib::SetValue(float float_value)
{
	char buffer[64] = {0};
	std::sprintf(buffer, "%f", float_value);
	value_ = buffer;
}
} // namespace bbk
