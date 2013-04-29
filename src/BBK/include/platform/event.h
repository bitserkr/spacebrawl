#ifndef _EVENT_H
#define _EVENT_H

#include "eventslist.h"

namespace bbk
{
/** @struct SysEvent
 *  @brief Encapsulates (operating) system events */
struct SysEvent
{
	union SysEventInfo
	{
		SysEventInfoAppWnd      appwnd;
		InputKey                key;
		SysEventInfoMouseMotion mousemove;
	};

	SysEventType type;
	SysEventInfo info;
}; // struct SysEvent
} // namespace bbk

#endif /* _EVENT_H */
