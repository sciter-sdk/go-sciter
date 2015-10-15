//
//  sciter-x-types.h
//  sciter
//
//  Created by andrew on 2014-03-29.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#ifndef sciter_sciter_x_types_h
#define sciter_sciter_x_types_h

#ifdef __cplusplus

  #if __cplusplus >= 201103L
    #define CPP11
  #elif _MSC_VER >= 1600
    #define CPP11
  #endif
  #include <string>
#endif


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
  #define LINUX
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

  #define SCAPI
  #define SCFN(name) (__stdcall *name)

  #define HWINDOW HWND
  #define SC_CALLBACK __stdcall

  typedef wchar_t wchar;

  #ifdef _WIN64
    #define TARGET_64
    #define SCITER_DLL_NAME "sciter64.dll"
  #else
    #define TARGET_32
    #define SCITER_DLL_NAME "sciter32.dll"
  #endif

#elif defined(OSX)

  //#ifdef __OBJC__
  //  #define char16_t uint16_t
  //#endif
  #ifndef BOOL
    typedef signed char BOOL;
  #endif
  typedef unsigned int UINT;
  typedef int INT;
  typedef unsigned long long UINT64;
  typedef int INT64;

  typedef unsigned char BYTE;
  typedef uint16_t WCHAR;
  typedef const WCHAR* LPCWSTR;
  typedef WCHAR* LPWSTR;
  typedef char CHAR;
  typedef const CHAR* LPCSTR;
  typedef void VOID;
  typedef size_t UINT_PTR;
  typedef void* LPVOID;
  typedef const void* LPCVOID;

  #define SCAPI  __cdecl
  #define SCFN(name) (__cdecl *name)
  #define SC_CALLBACK  __cdecl
  #define CALLBACK __cdecl

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
  typedef long LONG;

  typedef unsigned char BYTE;
  typedef wchar_t WCHAR;
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

  #ifdef __x86_64
    #define TARGET_64
    #define SCITER_DLL_NAME "sciter-gtk-64.so"
  #else
    #define TARGET_32
    #define SCITER_DLL_NAME "sciter-gtk-32.so"
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



