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

#ifdef __cplusplus

  #if __cplusplus >= 201103L
    #define CPP11
  #elif _MSC_VER >= 1600
    #define CPP11
  #endif
  #include <string>
#else

  #include <stdbool.h>
  #include <uchar.h>

#endif

enum GFX_LAYER
{
    GFX_LAYER_GDI = 1, GFX_LAYER_CG = 1, /*Mac OS*/ GFX_LAYER_CAIRO = 1, /*GTK*/
    GFX_LAYER_WARP = 2,
    GFX_LAYER_D2D = 3,
    GFX_LAYER_SKIA = 4,
    GFX_LAYER_SKIA_OPENGL = 5,
    GFX_LAYER_AUTO = 0xFFFF,
};


#if defined(_WIN32) || defined(_WIN64)

  #define WINDOWS

#elif defined(__APPLE__)
  #include "TargetConditionals.h"

  #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    #define IOS
  #elif TARGET_OS_MAC
    #define OSX
  #else
    #error "This platform is not supported yet"
  #endif

#elif defined(__linux__)
  #ifndef LINUX
  #define LINUX
  #endif
#else
  #error "This platform is not supported yet"
#endif

#if defined(WINDOWS)
  #include <specstrings.h>
  #include <windows.h>
  #include <oaidl.h>

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )
  #include <d2d1.h>
  #include <dwrite.h>
#else
  struct _ID2D1RenderTarget;
  struct _ID2D1Factory;
  struct _IDWriteFactory;

  typedef struct _ID2D1RenderTarget ID2D1RenderTarget;
  typedef struct _ID2D1Factory ID2D1Factory;
  typedef struct _IDWriteFactory IDWriteFactory;

#endif


  #if defined(_MSC_VER) && _MSC_VER < 1900
  // Microsoft has finally implemented snprintf in Visual Studio 2015.
  # define snprintf _snprintf_s
  # define vsnprintf vsnprintf_s
  #endif

  #if __STDC_WANT_SECURE_LIB__
  // use the safe version of `wcsncpy` if wanted
  # define wcsncpy wcsncpy_s
  #endif


  #ifdef STATIC_LIB
    void SciterInit( bool start);
  #else
    //#if defined(SCITER_EXPORTS)
    //  #define SCAPI __declspec(dllexport) __stdcall
    //#else
    //  #define SCAPI __declspec(dllimport) __stdcall
    //#endif
  #endif

  #define SCAPI __stdcall
  #define SCFN(name) (__stdcall *name)

  #define HWINDOW HWND
  #define SC_CALLBACK __stdcall

  typedef wchar_t wchar;

  #ifdef _WIN64
    #define TARGET_64
  #else
    #define TARGET_32
  #endif

#elif defined(OSX)

  //#ifdef __OBJC__
  //  #define char16_t uint16_t
  //#endif
  #ifndef BOOL
    typedef signed char BOOL;
  #endif
  #ifndef TRUE
    #define TRUE (1)
    #define FALSE (0)
  #endif

  typedef unsigned int UINT;
  typedef int INT;
  typedef unsigned long long UINT64;
  typedef int INT64;

  typedef unsigned char BYTE;
  typedef char16_t WCHAR;
  typedef const WCHAR* LPCWSTR;
  typedef WCHAR* LPWSTR;
  typedef char CHAR;
  typedef const CHAR* LPCSTR;
  typedef void VOID;
  typedef size_t UINT_PTR;
  typedef void* LPVOID;
  typedef const void* LPCVOID;

  #define SCAPI  
  #define SCFN(name) (*name)
  #define SC_CALLBACK 
  #define CALLBACK 



  typedef struct tagRECT
  {
     INT    left;
     INT    top;
     INT    right;
     INT    bottom;
  } RECT, *LPRECT;
  typedef const RECT * LPCRECT;

  typedef struct tagPOINT
  {
    INT  x;
    INT  y;
  } POINT, *PPOINT, *LPPOINT;

  typedef struct tagSIZE
  {
    INT        cx;
    INT        cy;
  } SIZE, *PSIZE, *LPSIZE;

  #define HWINDOW void*   // NSView*
  #define HINSTANCE void* // NSApplication*
  #define HDC void*       // CGContextRef

  #define LRESULT long


  #ifdef __LP64__
    #define TARGET_64
    #define SCITER_DLL_NAME "sciter-osx-64.dylib"
  #else
    #define TARGET_32
    #define SCITER_DLL_NAME "sciter-osx-32.dylib"
  #endif

#elif defined(LINUX)

  #include <gtk/gtk.h>
  #include <string.h>
  #include <wctype.h>

#ifndef BOOL
    typedef signed char BOOL;
  #endif
  typedef unsigned int UINT;
  typedef int INT;
  typedef unsigned long long UINT64;
  typedef int INT64;

  typedef unsigned char BYTE;
  typedef char16_t WCHAR;
  typedef const WCHAR* LPCWSTR;
  typedef WCHAR* LPWSTR;
  typedef char CHAR;
  typedef const CHAR* LPCSTR;
  typedef void VOID;
  typedef size_t UINT_PTR;
  typedef void* LPVOID;
  typedef const void* LPCVOID;

  #define SCAPI
  #define SCFN(name) (*name)
  #define SC_CALLBACK

  typedef struct tagRECT
  {
     INT    left;
     INT    top;
     INT    right;
     INT    bottom;
  } RECT, *LPRECT;
  typedef const RECT * LPCRECT;

  typedef struct tagPOINT
  {
    INT  x;
    INT  y;
  } POINT, *PPOINT, *LPPOINT;

  typedef struct tagSIZE
  {
    INT        cx;
    INT        cy;
  } SIZE, *PSIZE, *LPSIZE;

  #define HWINDOW GtkWidget* //
  #define HINSTANCE void*    //
  #define LRESULT long
  #define HDC void*       // cairo_t

  #ifdef __x86_64
    #define TARGET_64
    #define SCITER_DLL_NAME "libsciter-gtk-64.so"
  #else
    #define TARGET_32
    #define SCITER_DLL_NAME "libsciter-gtk-32.so"
  #endif


#endif


#if !defined(OBSOLETE)
/* obsolete API marker*/
  #if defined(__GNUC__)
    #define OBSOLETE __attribute__((deprecated))
  #elif defined(_MSC_VER) && (_MSC_VER >= 1300)
    #define OBSOLETE __declspec(deprecated)
  #else
    #define OBSOLETE
  #endif
#endif

#ifndef LPUINT
  typedef UINT* LPUINT;
#endif

#ifndef LPCBYTE
  typedef const BYTE* LPCBYTE;
#endif

/**callback function used with various get*** functions */
typedef VOID SC_CALLBACK LPCWSTR_RECEIVER( LPCWSTR str, UINT str_length, LPVOID param );
typedef VOID SC_CALLBACK LPCSTR_RECEIVER( LPCSTR str, UINT str_length, LPVOID param );
typedef VOID SC_CALLBACK LPCBYTE_RECEIVER( LPCBYTE str, UINT num_bytes, LPVOID param );

#define STDCALL __stdcall

#ifdef __cplusplus

  #define EXTERN_C extern "C"

  namespace std {
    typedef basic_string<WCHAR> ustring;
  }


  // Note: quote here is a string literal!
  #ifdef WINDOWS
    #define _WSTR(quote) L##quote
  #else
    #define _WSTR(quote) u##quote
  #endif
  #define WSTR(quote) ((const WCHAR*)_WSTR(quote))


#else
  #define EXTERN_C extern
#endif /* __cplusplus **/



#endif



