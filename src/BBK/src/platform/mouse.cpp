#include <cstring>
#include "sdl/SDL.h"
#include "mouse.h"

namespace
{
/** @struct ButtonState
 *  @brief  Bitfield of mouse button states */
struct ButtonState
{
	int isPressed     : 1;
	int isTriggered   : 1;
	int isReleased    : 1;
	int isDClicked    : 1;
	int dClickCounter : 16;
}; // struct ButtonState

int         x = 0, dx = 0;
int         y = 0, dy = 0;
int         prevStatus = 0;
int         currStatus = 0;
int         windowXMid = 0;
int         windowYMid = 0;
ButtonState status[bbk::MOUSE_NUM_CTRLS];
int         dClickThreshold(DCLICK_THRESHOLD);
} // anon namespace

namespace bbk
{
namespace mouse
{
bool Init()
{
	std::memset(::status, 0, sizeof(::status));
	for (int i = 0; i < MOUSE_NUM_CTRLS; ++i)
		::status[i].dClickCounter = ::dClickThreshold + 1;
	
	return true;
}

void Update()
{
	for (int i = 0; i < MOUSE_NUM_CTRLS; ++i)
	{
		::status[i].isPressed   =   (::currStatus & (1 << i)) >> i;
		::status[i].isTriggered = (((::currStatus ^ ::prevStatus) & ::currStatus) & (1 << i)) >> i;
		::status[i].isReleased  = (((::currStatus ^ ::prevStatus) & ::prevStatus) & (1 << i)) >> i;

		if (::status[i].isTriggered)
		{
			if (::status[i].dClickCounter < ::dClickThreshold)
				::status[i].isDClicked = 1;
			::status[i].dClickCounter = 0;
		}
		else
			::status[i].isDClicked = 0;
	}

	/* Prepare for next frame */
	// Copy status to previous frame
	::prevStatus = ::currStatus;
	
	// MouseWheelUp events are ignored, so we have to manually reset the
	// states here
	::currStatus &= ~(1 << (MOUSE_WHEELUP   - KB_NUM_KEYS));
	::currStatus &= ~(1 << (MOUSE_WHEELDOWN - KB_NUM_KEYS));

	for (int i = 0; i < MOUSE_NUM_CTRLS; ++i)
	{
		if (::status[i].dClickCounter <= ::dClickThreshold)
			++::status[i].dClickCounter;
	}
}

int IsPressed(InputKey buttonCode)   {return ::status[buttonCode - KB_NUM_KEYS].isPressed;}
int IsTriggered(InputKey buttonCode) {return ::status[buttonCode - KB_NUM_KEYS].isTriggered;}
int IsReleased(InputKey buttonCode)  {return ::status[buttonCode - KB_NUM_KEYS].isReleased;}
int IsDClicked(InputKey buttonCode)  {return ::status[buttonCode - KB_NUM_KEYS].isDClicked;}
int IsWheelUp()                          {return ::status[MOUSE_WHEELUP - KB_NUM_KEYS].isTriggered;}
int IsWheelDown()                        {return ::status[MOUSE_WHEELDOWN - KB_NUM_KEYS].isTriggered;}

void ClearStatus() {::dx = 0; ::dy = 0;}
	
void HandleEvent(const SysEvent &ev)
{
	switch(ev.type)
	{
	case SYS_MOUSE_MOVE:
		::x  = ev.info.mousemove.x;
		::y  = ev.info.mousemove.y;
		::dx = ev.info.mousemove.dx;
		::dy = ev.info.mousemove.dy;
		break;
	
	case SYS_MOUSE_BDOWN:
		::currStatus |= (1 << (ev.info.key - KB_NUM_KEYS));
		break;

	case SYS_MOUSE_BUP:
		/*
		 * SDL registers a mousewheel movement as 2 consec events:
		 * Buttondown and then buttonup. Ignore the buttonup for wheels.
		 */
		if ((ev.info.key == MOUSE_WHEELUP) ||
			(ev.info.key == MOUSE_WHEELDOWN))
			break;

		::currStatus &= ~(1 << (ev.info.key - KB_NUM_KEYS));
		
		break;
	}
}

void SetPosition(int x, int y)
{
	SDL_WarpMouse(static_cast<unsigned short>(x), static_cast<unsigned short>(y));
}

void SetPosition_X(int x)
{
	SDL_WarpMouse(static_cast<unsigned short>(x), static_cast<unsigned short>(::y));
}

void SetPosition_Y(int y)
{
	SDL_WarpMouse(static_cast<unsigned short>(::x),static_cast<unsigned short>(y));
}

void SetWindowDimensions(int width, int height)
{
	::windowXMid = width  / 2;
	::windowYMid = height / 2;
}

int GetX()      {return ::x;}
int GetY()      {return ::y;}
int GetDeltaX() {return ::dx;}
int GetDeltaY() {return ::dy;}
} // namespace mouse
} // namespace bbk
