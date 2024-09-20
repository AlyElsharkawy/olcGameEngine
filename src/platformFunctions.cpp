#include "platformFunctions.h"

#if defined(OLC_PLATFORM_X11)
  using namespace X11;
  #include <X11/Xlib.h>
  #include <X11/extensions/Xfixes.h>
#endif

void CaptureMouse(olc::PixelGameEngine* engine)
{
  //Linux
  #ifdef OLC_PLATFORM_X11
  Display* display = XOpenDisplay(nullptr);
  XFixesHideCursor(display, DefaultRootWindow(display));  
  #endif

  //Windows
  #ifdef OLC_PLATFORM_WINAPI
  ShowCursor(false);
  #endif
}

void ReleaseMouse(olc::PixelGameEngine* engine)
{
  //Linux
  #if defined(OLC_PLATFORM_X11)
  #endif
  //Windows
  #if defined(OLC_PLATFORM_WINAPI)
  ShowCursor(true);
  #endif
}

