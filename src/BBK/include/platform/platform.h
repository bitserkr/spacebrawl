#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "clock.h"
#include "event.h"
#include "keyboard.h"
#include "mouse.h"
#include "pollster.h"
#include "appwindow.h"

namespace bbk
{
/// Initialise OS utility classes
bool InitPlatform();
/// Unload OS utility classes
void HaltPlatform();
} // namespace bbk

#endif /* _PLATFORM_H */
