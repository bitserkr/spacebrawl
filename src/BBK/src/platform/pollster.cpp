#include "sdl/SDL_events.h"
#include "pollster.h"

namespace
{
std::vector<bbk::SysEvent> eventVec;
} // anon namespace

namespace bbk
{
namespace pollster
{
/***************************************************************************//**
 * Checks system for existing events in queue. If any exist, the appropriate
 * attribute(s)/behaviour is updated/executed.
 * @note Called once every gameloop.
 *
 * @return
 *	(int) Number of events processed this gameloop.
*******************************************************************************/
int Poll()
{
	::eventVec.clear();

	SDL_Event newEvent; // struct that will hold event information
	int numEvents = 0;

	// Poll for all existing events, if any
	while (SDL_PollEvent(&newEvent))
	{
		++numEvents;
		SysEvent ev;

		// Determine type of event
		switch (newEvent.type)
		{
		// Application window events
		case SDL_VIDEORESIZE:
			ev.type = SYS_APPWND_RESIZE;
			break;
		
		case SDL_ACTIVEEVENT:
			ev.type = SYS_APPWND_ACTIVE;
			
			if (newEvent.active.state & SDL_APPINPUTFOCUS)
			{
				ev.info.appwnd =
					newEvent.active.gain ?
					SYS_APPWND_INPUTFOCUS_GAIN :
					SYS_APPWND_INPUTFOCUS_LOSS;
			}
			else if (newEvent.active.state & SDL_APPMOUSEFOCUS)
			{
				ev.info.appwnd =
					newEvent.active.gain ?
					SYS_APPWND_MOUSEFOCUS_GAIN :
					SYS_APPWND_MOUSEFOCUS_LOSS;
			}
			break;
		
		// Keyboard events
		case SDL_KEYDOWN:
			ev.type = SYS_KB_KEYDOWN;
			ev.info.key = static_cast<InputKey>(newEvent.key.keysym.sym);
			//ev.info.keypress = static_cast<DevKeybKey>(newEvent.key.keysym.sym);
			break;
		
		case SDL_KEYUP:
			ev.type = SYS_KB_KEYUP;
			ev.info.key = static_cast<InputKey>(newEvent.key.keysym.sym);
			//ev.info.keypress = static_cast<DevKeybKey>(newEvent.key.keysym.sym);
			break;

		// Mouse events
		case SDL_MOUSEMOTION:
			ev.type = SYS_MOUSE_MOVE;
			ev.info.mousemove.x  = newEvent.motion.x;
			ev.info.mousemove.y  = newEvent.motion.y;
			ev.info.mousemove.dx = newEvent.motion.xrel;
			ev.info.mousemove.dy = newEvent.motion.yrel;
			break;

		case SDL_MOUSEBUTTONDOWN:
			ev.type = SYS_MOUSE_BDOWN;
			ev.info.key = static_cast<InputKey>(newEvent.button.button - 1 + MOUSE_LMB);
			//ev.info.mousebutton = static_cast<DevMouseCtrl>(newEvent.button.button - 1);
			break;
		
		case SDL_MOUSEBUTTONUP:
			ev.type = SYS_MOUSE_BUP;
			ev.info.key = static_cast<InputKey>(newEvent.button.button - 1 + MOUSE_LMB);
			break;

		case SDL_VIDEOEXPOSE:
			ev.type = SYS_WINDOW_EXPOSE;
			break;

		// Quit event; user has force-quit application
		case SDL_QUIT:
			ev.type = SYS_APP_QUIT;
			break;

		default:
			ev.type = SYS_UNKNOWN;
			break;
		}

		::eventVec.push_back(ev);
	}
	return numEvents;
}

const std::vector<SysEvent>& GetEventsVec()
{
	return ::eventVec;
}
} // namespace pollster
} // namespace bbk
