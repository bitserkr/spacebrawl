#ifndef _COLOUR_H
#define _COLOUR_H

namespace bbk
{
struct Colour
{
	Colour(float argR = 1.0f, float argG = 1.0f, float argB = 1.0f, float argA = 1.0f) :
		r(argR), g(argG), b(argB), a(argA) {}
	
	float r, g, b, a;
};
} // namespace bbk

#endif /* _COLOUR_H */
