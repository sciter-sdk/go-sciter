/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

/*
 * Sciter basic types, platform isolation declarations
 */

#ifndef sciter_sciter_x_types_h
#define sciter_sciter_x_types_h

#include "sciter-x-primitives.h"

enum GFX_LAYER
{
    GFX_LAYER_GDI = 1, GFX_LAYER_CG = 1, /*Mac OS*/ GFX_LAYER_CAIRO = 1, /*GTK*/
    GFX_LAYER_WARP = 2, GFX_LAYER_D2D_WARP = 2,
    GFX_LAYER_D2D = 3,
    GFX_LAYER_SKIA = 4,
    GFX_LAYER_SKIA_OPENGL = 5,
    GFX_LAYER_AUTO = 0xFFFF,
};

#if defined(SCITER_LITE)
  #define WINDOWLESS
#endif

#if defined(WINDOWS)

  #ifdef STATIC_LIB
    void SciterInit( bool start);
  #endif

  #if defined(WINDOWLESS)
    #define HWINDOW LPVOID
  #else 
    #define HWINDOW HWND  
  #endif

  #define SCITER_DLL_NAME "sciter.dll"

#elif defined(OSX)

  #define HWINDOW LPVOID   // NSView*
  #define HINSTANCE LPVOID // NSApplication*
  #define HDC void*        // CGContextRef

  #define LRESULT long

  #define SCITER_DLL_NAME "libsciter.dylib"

#elif defined(LINUX)

  #if !defined(WINDOWLESS)
    #include <gtk/gtk.h>
  #endif

  #if defined(WINDOWLESS)
    #define HWINDOW void * 
  #else 
    #define HWINDOW GtkWidget* //
  #endif
  
  #define HINSTANCE LPVOID //
  #define LRESULT long
  #define HDC LPVOID       // cairo_t

  #if defined(WINDOWLESS)
    #define SCITER_DLL_NAME "libsciter.so"
  #else
    #define SCITER_DLL_NAME "libsciter-gtk.so"
  #endif

#elif defined(ANDROID)

  #define WINDOWLESS

  #define HWINDOW LPVOID

  #define HINSTANCE LPVOID //
  #define LRESULT long
  #define HDC LPVOID // not used anyway, draws on OpenGLESv2

  #define SCITER_DLL_NAME "libsciter.so"

#endif

#endif
