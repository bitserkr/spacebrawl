#ifndef _MOUSE_H
#define _MOUSE_H

/* Includes *******************************************************************/
#include "event.h"

/* Constants ******************************************************************/
#define DCLICK_THRESHOLD 20 /* In number of frames */

namespace bbk
{
namespace mouse
{
bool Init();
void Update();

/** @name
 *  Status queries *///\{
int IsPressed(InputKey buttonCode);
int IsTriggered(InputKey buttonCode);
int IsReleased(InputKey buttonCode);
int IsDClicked(InputKey buttonCode);
int IsWheelUp();
int IsWheelDown();
//\}

/** @name
 *  Input manager controller interface *///\{
void ClearStatus();
void HandleEvent(const SysEvent &ev);
//\}
	
/** @name
 *  Settors *///\{
void SetPosition(int x, int y);
void SetPosition_X(int x);
void SetPosition_Y(int y);
void SetWindowDimensions(int width, int height);
/// Responsibility of input module to set this according to delta time
void SetDClickThres(int numFrames);
//\}
	
/** @name
 *  Accessors *///\{
int GetX();
int GetY();
int GetDeltaX();
int GetDeltaY();
//\}
} // namespace mouse
} // namespace bbk

#endif /* _MOUSE_H */
