#include "sdl/SDL.h"       // SetVideoMode
#include "sdl/SDL_syswm.h" // GetWMInfo
#include "appwindow.h"
#include "graphics/graphics.h"

namespace
{
bbk::Drawable*            pScreen        = nullptr;
bbk::AppWndInfo*          pWndInfo       = nullptr;
bbk::appwindow::GFX_ACCEL gfxtype        = bbk::appwindow::SOFTWARE;
bool                      isFullscr      = false;
int                       width          = 0;
int                       height         = 0;
int                       pixelDepth     = 32;
unsigned                  videoFlags     = 0;
bool                      bCursorVisible = false;
bool                      inputConfined  = true;
} // anon namespace

namespace bbk
{
namespace appwindow
{
bool Init()
{
	return true;
}

void Halt()
{
	if (::pWndInfo)
	{
		delete ::pWndInfo;
		::pWndInfo = nullptr;
	}
	SDL_FreeSurface(::pScreen);
	::pScreen = nullptr;
}

void SwapFramebuffers()
{
	if (::videoFlags & SDL_OPENGL)
		SDL_GL_SwapBuffers();
}

bool OpenWindow(const int inWidth, const int inHeight, const GFX_ACCEL ingfxtype, const bool inIsFullscr)
{
	// Save config
	::gfxtype   = ingfxtype;
	::isFullscr = inIsFullscr;

	::videoFlags = SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE;
	if (::isFullscr)
		::videoFlags |= SDL_FULLSCREEN;
	if (::gfxtype == OPENGL)
		::videoFlags |= SDL_OPENGL;
	
	// Center windowed app
	SDL_putenv("SDL_VIDEO_CENTERED=center");
	
	// Create app window
	::pScreen = SDL_SetVideoMode(
		inWidth,
		inHeight,
		::pixelDepth,
		::videoFlags);

	if (::pScreen)
	{
		// Save viewport attributes
		::width = ::pScreen->w;
		::height = ::pScreen->h;
		::pixelDepth = ::pScreen->format->BitsPerPixel;

		// Save window handle
		::pWndInfo = new SDL_SysWMinfo;
		SDL_VERSION(&::pWndInfo->version);
		SDL_GetWMInfo(::pWndInfo);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  24);

		return bbk::gfx::InitGL() ? true : false;
	}
	return false;
}

void ResizeWindow(const int width, const int height)
{
	if (::isFullscr)
		::videoFlags |= SDL_FULLSCREEN;
	else
		::videoFlags &= ~SDL_FULLSCREEN;

	::pScreen = SDL_SetVideoMode(
		::isFullscr ? 0 : width,
		::isFullscr ? 0 : height,
		::pixelDepth,
		::videoFlags);

	if (::pScreen)
	{
		// Save viewport attributes
		::width = ::pScreen->w;
		::height = ::pScreen->h;
		::pixelDepth = ::pScreen->format->BitsPerPixel;
	}
}

void SetTitle(const char * const str)
{
	SDL_WM_SetCaption(str, nullptr);
}

int  GetWindowWidth()  {return ::width;}
int  GetWindowHeight() {return ::height;}
bool IsFullscreen()    {return ::isFullscr;}

void SetDimensions(const int inWidth, const int inHeight)
{
	// Do nothing if requested dimension is not supported
	if (SDL_VideoModeOK(inWidth, inHeight, ::pixelDepth, ::videoFlags) == 0)
		return;

	::width  = inWidth;
	::height = inHeight;

	::pScreen = SDL_SetVideoMode(
		::width,
		::height,
		::pixelDepth,
		::videoFlags);
}

void SetFullscreen(bool flag)
{
	if (flag == ::isFullscr)
		return;

	::isFullscr = flag;
	ResizeWindow(::width, ::height);
}

int   GetWindowBitsPerPixel() {return ::pixelDepth;}
void* GetWin32HWND()          {return ::pWndInfo->window;}

bool IsCursorVisible() {return ::bCursorVisible;}

void SetCursorVisibility(bool inbCursorVisible)
{
	::bCursorVisible = inbCursorVisible;
	SDL_ShowCursor(::bCursorVisible ? SDL_ENABLE : SDL_DISABLE);
}

void ConfineCursor(bool flag)
{
	::inputConfined = flag;
	SDL_WM_GrabInput(flag ? SDL_GRAB_ON : SDL_GRAB_OFF);
}
} // namespace appwindow
} // namespace bbk
