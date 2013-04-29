#include "sdl/SDL.h"
#include "platform.h"

namespace bbk
{
bool InitPlatform()
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
		return false;

	if (!keyboard::Init())
		return false;
	if (!mouse::Init())
		return false;

	return true;
}

void HaltPlatform()
{
	appwindow::Halt();
	SDL_Quit();
}
} // namespace bbk
