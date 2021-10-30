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

#ifndef sciter_sciter_x_primitives_h
#define sciter_sciter_x_primitives_h

#ifdef __cplusplus

  #if __cplusplus >= 201103L
    #define CPP11
  #elif _MSC_VER >= 1600
    #define CPP11
  #endif
  #include <string>
#else

  #include <stdbool.h>

#endif

#ifndef SBOOL
  typedef int SBOOL;
#endif
#ifndef TRUE
  #define TRUE (1)
  #define FALSE (0)
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

#elif defined(__ANDROID__)
  #ifndef ANDROID
    #define ANDROID
  #endif
  #ifndef WINDOWLESS
   #define WINDOWLESS
  #endif
#elif defined(__linux__)
  #ifndef LINUX
    #define LINUX
  #endif
#else
  #error "This platform is not supported yet"
#endif

#if defined(WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #define _WINSOCKAPI_
  #include <specstrings.h>
  #include <windows.h>
  #include <oaidl.h>

  #if defined(_MSC_VER) && _MSC_VER < 1900
  // Microsoft has finally implemented snprintf in Visual Studio 2015.
    #define snprintf _snprintf_s
    #define vsnprintf vsnprintf_s
  #endif

  //#if __STDC_WANT_SECURE_LIB__
  //// use the safe version of `wcsncpy` if wanted
  //  #define wcsncpy wcsncpy_s
  //#endif

  #define SCAPI __stdcall
  #define SCFN(name) (__stdcall *name)

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

  #include <stddef.h>
  #ifndef _UINT16_T
  #define _UINT16_T
  typedef unsigned short uint16_t;
  #endif

  #ifndef _CHAR16_T
  #define _CHAR16_T
  typedef uint16_t char16_t;
  #endif

  typedef unsigned int UINT;
  typedef int INT;
  typedef unsigned int UINT32;
  typedef int INT32;
  typedef unsigned long long UINT64;
  typedef long long INT64;

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

#elif defined(LINUX)

  #include <string.h>
  #include <wctype.h>

  typedef unsigned int UINT;
  typedef int INT;
  typedef unsigned int UINT32;
  typedef int INT32;
  typedef unsigned long long UINT64;
  typedef long long INT64;

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


  #if defined(ARM) || defined(__arm__)
    #define TARGET_ARM
  #elif defined(__x86_64)
    #define TARGET_64
  #else
    #define TARGET_32
  #endif

#elif defined(ANDROID)

  #define WINDOWLESS

  //#include <uchar.h>
  #include <string.h>

  #ifndef SBOOL
  typedef signed int        SBOOL;
  #endif
  #ifndef TRUE
  #define TRUE (1)
  #define FALSE (0)
  #endif
  typedef unsigned int       UINT;
  typedef int                INT;
  typedef unsigned long long UINT64;
  typedef long long          INT64;

  typedef unsigned char BYTE;
  typedef char16_t      WCHAR;
  typedef const WCHAR * LPCWSTR;
  typedef WCHAR *       LPWSTR;
  typedef char          CHAR;
  typedef const CHAR *  LPCSTR;
  typedef void          VOID;
  typedef size_t        UINT_PTR;
  typedef void *        LPVOID;
  typedef const void *  LPCVOID;

  #define SCAPI
  #define SCFN(name) (*name)
  #define SC_CALLBACK

  typedef struct tagRECT {
    INT left;
    INT top;
    INT right;
    INT bottom;
  } RECT, *LPRECT;
  typedef const RECT *LPCRECT;

  typedef struct tagPOINT {
    INT x;
    INT y;
  } POINT, *PPOINT, *LPPOINT;

  typedef struct tagSIZE {
    INT cx;
    INT cy;
  } SIZE, *PSIZE, *LPSIZE;

  #if defined(ARM) || defined(__arm__)
  #define TARGET_ARM
  #elif defined(__x86_64)
  #define TARGET_64
  #else
  #define TARGET_32
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

  inline VOID SC_CALLBACK _LPCBYTE2ASTRING(LPCBYTE bytes, UINT num_bytes, LPVOID param)
  {
    std::string* s = (std::string*)param;
    *s = std::string((const char*)bytes, num_bytes);
  }
  inline VOID SC_CALLBACK _LPCWSTR2STRING(LPCWSTR str, UINT str_length, LPVOID param)
  {
    std::ustring* s = (std::ustring*)param;
    *s = std::ustring(str, str_length);
  }
  inline VOID SC_CALLBACK _LPCSTR2ASTRING(LPCSTR str, UINT str_length, LPVOID param)
  {
    std::string* s = (std::string*)param;
    *s = std::string(str, str_length);
  }
  
#else
  #define EXTERN_C extern
#endif /* __cplusplus **/


#endif



