#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "event.h"

namespace bbk
{
namespace keyboard
{
bool Init();
void Update();

/**
 * \name
 * Keystate queries.
 *///\{
/// Gets status of a specific key, with optional logical sum of modifier keys
int IsKeyPressed(int key, int modLogicalSum = 0);
/// Checks if a key was just pressed this gameloop, with optional logical sum of modifier keys
int IsKeyTriggered(int key, int modLogicalSum = 0);
/// Checks if a key was just released this gameloop, with optional logical sum of modifier keys
int IsKeyReleased(int key, int modLogicalSum = 0);
//\}
	
/**
 * \name
 * Keyboard meta-functions
 *///\{
/// Turns on keypress repeat
int  EnableRepeat();
/// Turns off keypress repeat
int  DisableRepeat();
/// Sets time interval to wait between first keypress and start of repeats
void SetRepeatDelay(const int ms);
/// Sets time interval between subsequent repeating keypress events
void SetRepeatInterval(const int ms);
/// Sets keyboard delay to be suitable for real-time input
void SetDelayRealTime();
/// Sets keyboard delay to be suitable for text input
void SetDelayText();
/// Disables keyboard detection for all but the active text form
void Lock();
/// Allows keypress detection by all
void Unlock();
//\}

/// Event handling
void HandleEvent(const SysEvent &ev);
} // namespace keyboard
} // namespace bbk

#endif /* _KEYBOARD_H */
