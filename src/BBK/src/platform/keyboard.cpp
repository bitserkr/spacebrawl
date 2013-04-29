#include <cstring>            /* memset */
#include "sdl/SDL_keyboard.h"
#include "keyboard.h"

namespace
{
const int DEFAULT_REPEAT_DELAY    = 500; // in milliseconds
const int DEFAULT_REPEAT_INTERVAL = 30;  // in milliseconds

/** @struct KeyState
 *  @brief  Bitfield of keyboard key state */
struct KeyState
{
	int prevState   : 1;
	int currState   : 1;
	int textState   : 1;
	int isTriggered : 1;
	int isReleased  : 1;
}; // struct KeyState

KeyState status[bbk::KB_NUM_KEYS];
int      modStates      = 0;
int      repeatDelay    = DEFAULT_REPEAT_DELAY;
int      repeatInterval = DEFAULT_REPEAT_INTERVAL;
bool     locked         = false;

void ClearStatusArray();
} // anon namespace

namespace bbk
{
namespace keyboard
{
/***************************************************************************//**
 * Initialises SysKeyboard object attributes and creates a char array that will
 * hold a string of all printable characters typed on the keyboard.
 *
 * \note The initial capacity of this string has a default of whichever value
 * was specified in header or int constructor of object.
 *
 * \return
 *	(bool) true if memory was allocated to text buffer; false otherwise.
*******************************************************************************/
bool Init()
{
	// Default keypress repeat values
	::repeatDelay    = ::DEFAULT_REPEAT_DELAY;
	::repeatInterval = ::DEFAULT_REPEAT_INTERVAL;
	EnableRepeat();
	ClearStatusArray();

	return true;
}

void Update()
{
	for (int i = 0; i < KB_NUM_KEYS; ++i)
	{
		::status[i].isTriggered = (::status[i].currState ^ ::status[i].prevState) & ::status[i].currState;
		::status[i].isReleased  = (::status[i].currState ^ ::status[i].prevState) & ::status[i].prevState;
		::status[i].prevState   =  ::status[i].currState;
	}

	::modStates = 0;
	if (::status[KB_LSHIFT].currState)
		::modStates |= KB_MOD_LSHIFT;
	if (::status[KB_RSHIFT].currState)
		::modStates |= KB_MOD_RSHIFT;
	if (::status[KB_LCTRL].currState)
		::modStates |= KB_MOD_LCTRL;
	if (::status[KB_RCTRL].currState)
		::modStates |= KB_MOD_RCTRL;
	if (::status[KB_LALT].currState)
		::modStates |= KB_MOD_LALT;
	if (::status[KB_RALT].currState)
		::modStates |= KB_MOD_RALT;
}

/* Doxygen block comment to create page for keyboard/mouse enum values *//**
 * \page DevKeybEnum DevKeybKey enum reference
 * \include DevKeybEnum.h
 */
/***************************************************************************//**
 * Gets status of a specific keyboard key.
 *
 * \param[in] keycode
 *	(DevKeybKey) enum value that represents specific key to check.
 *	\ref InputDevEnum
 *
 * \return
 *	(int) 1 if key is pressed/down; 0 if up.
*******************************************************************************/
int IsKeyPressed(int keycode, int modLogicalSum)
{
	return modLogicalSum ?
		(::status[keycode].currState && (modLogicalSum & ::modStates)) :
		::status[keycode].currState;
}

/***************************************************************************//**
 * Checks if a key was just pressed this gameloop.
 *
 * \param[in] keycode
 *	(DevKeybKey) enum value that represents specific key to check.
 *	\ref InputDevEnum
 *
 * \return
 *	(bool) true if key was just pressed this loop; false if not.
*******************************************************************************/
int IsKeyTriggered(int keycode, int modLogicalSum)
{
	if (modLogicalSum)
	{
		// If modifier key requested, check for both
		return ::status[keycode].isTriggered && (modLogicalSum & ::modStates);
	}
	else
	{
		// No modifier key must be pressed
		return ::modStates ? 0 : ::status[keycode].isTriggered;
	}
}

/***************************************************************************//**
 * Checks if a key was just released this gameloop.
 *
 * \param[in] keycode
 *	(DevKeybKey) enum value that represents specific key to check.
 *	\ref InputDevEnum
 *
 * \return
 *	(bool) true if key was just released this loop; false if not.
*******************************************************************************/
int IsKeyReleased(int keycode, int modLogicalSum)
{
	return modLogicalSum ?
		(::status[keycode].isReleased && (modLogicalSum & ::modStates)) :
		::status[keycode].isReleased;
}

/***************************************************************************//**
 * Enables repeating registration of keypresses when a key is held down.
 * (Multiple keydown events when holding down a key.)
 * \note Also used to apply new delay/interval attributes. It is
 * 		not assumed that user will want to apply keyboard repeat immediately
 * 		after setting delay/interval values, so it is left to the programmer to
 * 		call this function after changing those values to apply any changes.
 *
 * \return
 *	(int) 0 on success; -1 on failure.
*******************************************************************************/
int EnableRepeat()
{
	return SDL_EnableKeyRepeat(::repeatDelay, ::repeatInterval);
}

/***************************************************************************//**
 * Disables repeating registration of keypresses when a key is held down.
 *
 * \return
 *	(int32_t) 0 on success; -1 on failure.
*******************************************************************************/
int DisableRepeat()
{
	return SDL_EnableKeyRepeat(0, 0);
}

/***************************************************************************//**
 * Sets the time interval to wait <b>after the first</b> keypress before
 * starting to repeat keydown events.
 *
 * \param[in] delay
 *	(int32_t) Delay interval, in milliseconds.
 *
 * \return
 *	Nothing.
*******************************************************************************/
void SetRepeatDelay(const int delay)
{
	::repeatDelay = delay;
}

/***************************************************************************//**
 * Sets the time interval between repeating keydown events <b>after the first
 * </b>\e repeating keypress.
 *
 * \param[in] interval
 *	(int32_t) Delay interval between keydown events, in milliseconds.
 *
 * \return
 *	Nothing.
*******************************************************************************/
void SetRepeatInterval(const int interval)
{
	::repeatInterval = interval;
}

void SetDelayRealTime()
{
	// Do not allow change if an active text form is hogging keyboard
	if (::locked)
		return;
	SetRepeatDelay(1);
	SetRepeatInterval(1);
	EnableRepeat();  // Apply change
}

void SetDelayText()
{
	// Do not allow change if an active text form is hogging keyboard
	if (::locked)
		return;
	SetRepeatDelay(DEFAULT_REPEAT_DELAY);
	SetRepeatInterval(DEFAULT_REPEAT_INTERVAL);
	EnableRepeat();  // Apply change
}

/***************************************************************************//**
 * Prevents function calls checking keypress status from all but the
 * active text form from returning true even if so.
 * This is for active text forms to hog the keyboard.
 * \warning _MUST_ be paired with an Unlock call.
 *
 * \return
 *	Nothing.
*******************************************************************************/
void Lock()
{
	::locked = true;
}

/***************************************************************************//**
 * Returns keyboard check functions to normal
 *
 * \return
 *	Nothing.
*******************************************************************************/
void Unlock()
{
	::locked = false;
}

void HandleEvent(const SysEvent &ev)
{
	switch(ev.type)
	{
	case SYS_KB_KEYDOWN:
		::status[ev.info.key].currState = 1;
		break;
	
	case SYS_KB_KEYUP:
		::status[ev.info.key].currState = 0;
		break;
	}
}
} // namespace keyboard
} // namespace bbk

namespace
{
void ClearStatusArray()
{
	std::memset(::status, 0, sizeof(::status));
}
} // anon namespace
