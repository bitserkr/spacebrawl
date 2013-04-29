#ifndef _EVENTSLIST_H
#define _EVENTSLIST_H

#include <cstdint>         /* uint16_t, int16_t */
#include "inputkeys.h"

namespace bbk
{
/**
 * \enum  SysEventType
 * \brief List of platform event-types.
 */
enum SysEventType
{
	SYS_APPWND_RESIZE,
	SYS_APPWND_ACTIVE,
	SYS_KB_KEYDOWN,
	SYS_KB_KEYUP,
	SYS_MOUSE_MOVE,
	SYS_MOUSE_BDOWN,
	SYS_MOUSE_BUP,
	SYS_WINDOW_EXPOSE,
	SYS_APP_QUIT,
	SYS_UNKNOWN,

	SYS_NUM_EVENTS
};

/**
 * \name
 * Information about each event type, or subevent.
 *///\{
/** \enum SysEventAppEndInfo \brief For SYS_APPWND_ACTIVE event */
enum SysEventInfoAppWnd
{
	SYS_APPWND_INPUTFOCUS_GAIN,
	SYS_APPWND_INPUTFOCUS_LOSS,
	SYS_APPWND_MOUSEFOCUS_GAIN,
	SYS_APPWND_MOUSEFOCUS_LOSS
};

///** \struct SysEventInpKeyInfo \brief For SYS_KB_ and SYS_MOUSE_B events */
//struct SysEventInfoInpKey
//{
//	InpDevKey key;
//};

struct SysEventInfoMouseMotion
{
	uint16_t x, y;
	int16_t  dx, dy;
}; // struct SysEventInfoMouseMotion
//\}
} // namespace bbk

#endif /* _EVENTSLIST_H */
