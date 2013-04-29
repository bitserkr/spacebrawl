#ifndef _APPWINDOW_H
#define _APPWINDOW_H

/* Typedefs/structs ***********************************************************/
struct SDL_Surface;
struct SDL_SysWMinfo;

namespace bbk
{
typedef SDL_Surface   Drawable;
typedef SDL_SysWMinfo AppWndInfo;

namespace appwindow
{
enum GFX_ACCEL {SOFTWARE, OPENGL, OTHER_API};

bool Init();
void Halt();

void SwapFramebuffers();

bool OpenWindow(const int width, const int height, const GFX_ACCEL gfxtype, const bool isFullscreen);
void ResizeWindow(const int width, const int height);
void SetTitle(const char * const title);

/** @name
 *  Application window dimension methods *///\{
int  GetWindowWidth();
int  GetWindowHeight();
bool IsFullscreen();
	
void SetDimensions(const int width, const int height);
void SetFullscreen(bool flag);
//\}

/** @name
 *  System accessors *///\{
int   GetWindowBitsPerPixel();
void* GetWin32HWND();
//\}

/** @name
 *  Pointing-device cursor methods *///\{
bool IsCursorVisible();
	
void SetCursorVisibility(bool bCursorVisible);
void ConfineCursor(bool flag);
//\}
} // namespace appwindow
} // namespace bbk

#endif /* _APPWINDOW_H */
