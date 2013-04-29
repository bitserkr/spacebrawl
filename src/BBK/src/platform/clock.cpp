#include "sdl/SDL_timer.h"
#include "clock.h"

namespace
{
uint32_t tickCount     = 0;
uint32_t prevTickCount = 0;
uint32_t deltaTicks    = 0;
} // anon namespace

namespace bbk
{
namespace clock
{
//void Update()
//{
//	tickCount_     = SDL_GetTicks();
//	deltaTicks_    = tickCount_ - prevTickCount_;
//	prevTickCount_ = tickCount_;
//}

uint32_t GetTicks()
{
	::tickCount     = SDL_GetTicks();
	::deltaTicks    = ::tickCount - ::prevTickCount;
	::prevTickCount = ::tickCount;
	return ::tickCount;
}

uint32_t GetDeltaTicks()
{
	return ::deltaTicks;
}

} // namespace clock
} // namespace bbk
