#ifndef __sciter_aux_h__
#define __sciter_aux_h__

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

/*
 * Terra Informatica Sciter and HTMLayout Engines
 * http://terrainformatica.com/sciter, http://terrainformatica.com/htmlayout
 *
 * basic primitives.
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief primitives
 **/

/*

  pod::copy<T> - memcpy wrapper
  pod::move<T> - memmove wrapper
  pod::buffer<T> - dynamic buffer, string builder, etc.

  utf8::towcs() - utf8 to WCHAR* converter
  utf8::fromwcs() - WCHAR* to utf8 converter
  utf8::ostream  - raw ASCII/UNICODE -> UTF8 converter
  utf8::oxstream - ASCII/UNICODE -> UTF8 converter with XML support

  inline bool streq(const char* s, const char* s1) - NULL safe string comparison function
  inline bool wcseq(const WCHAR* s, const WCHAR* s1) - NULL safe wide string comparison function
  inline bool streqi(const char* s, const char* s1) - the same, but case independent
  inline bool wcseqi(const WCHAR* s, const WCHAR* s1) - the same, but case independent

  w2a - helper object for const WCHAR* to const char* conversion
  a2w - helper object for const char* to const WCHAR* conversion
  w2utf - helper object for const WCHAR* to utf8 conversion
  utf2w - helper object for utf8 to const WCHAR* conversion

  t2w - const TCHAR* to const WCHAR* conversion, #definition
  w2t - const WCHAR* to const TCHAR* conversion, #definition

  itoa, itow - int to const char* converter
  atoi, wtoi - const char* to int converter (parser)
  ftoa, ftow - double to const char* converter


 */

#pragma once

#include <assert.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>    // std::reverse
#include "aux-platform.h"
#include "aux-slice.h"

#include <string>
#include <algorithm>

// disable that warnings in VC 2005
#pragma warning(disable:4786) //identifier was truncated...
#pragma warning(disable:4100) //unreferenced formal parameter

#ifndef BYTE
  typedef unsigned char   BYTE;
#endif

//#include "aux-slice.h"

// WARNING: macros below must be used only for passing parameters to functions!

/*
#if !defined(W2A) // WCHAR to multi-BYTE string converter (current locale)
#define W2A aux::w2a
#endif

#if !defined(A2W) // multi-BYTE to WCHAR string converter (current locale)
#define A2W aux::a2w
#endif

#if !defined(UTF2W) // utf-8 to WCHAR string converter
#define UTF2W aux::utf2w
#endif

#if !defined(W2UTF) // WCHAR to utf-8 string converter
#define W2UTF aux::w2utf
#endif

#if !defined(W2T)
#if !defined(UNICODE)
#define W2T(S) aux::w2a(S)
#else
#define W2T(S) (S)
#endif
#endif

#if !defined(T2W)
#if !defined(UNICODE)
#define T2W(S) aux::a2w(S)
#else
#define T2W(S) (S)
#endif
#endif

#if !defined(A2T)
#if !defined(UNICODE)
#define A2T(S) (S)
#else
#define A2T(S) aux::a2w(S)
#endif
#endif

#if !defined(T2A)
#if !defined(UNICODE)
#define T2A(S) (S)
#else
#define T2A(S) aux::w2a(S)
#endif
#endif

*/

#ifdef UNICODE
#define a2t( S ) aux::a2w(S)
#define t2a( S ) aux::w2a(S)
#define w2t( S ) (S)
#define t2w( S ) (S)
#define t2i( S ) aux::wtoi(S,0)
#define i2t( S ) aux::itow(S)
#else
#define a2t( S ) (S)
#define t2a( S ) (S)
#define w2t( S ) aux::w2a(S)
#define t2w( S ) aux::a2w(S)
#define t2i( S ) aux::atoi(S,0)
#define i2t( S ) aux::itoa(S)
#endif

#define w2u( S ) aux::w2utf(S)
#define u2w( S ) aux::utf2w(S)

#define i2a( I ) aux::itoa(I)
#define i2w( I ) aux::itow(I)

inline void* zalloc ( size_t sz)
{
    void* p = malloc(sz);
    if(p) memset(p,0,sz);
    return p;
}

//elements in array literal
#define ITEMS_IN(a) (sizeof(a)/sizeof(a[0]))
//chars in sting literal
#define CHARS_IN(s) (sizeof(s) / sizeof(s[0]) - 1)

/**pod namespace - POD primitives. **/
namespace pod
{
  template <typename T> void copy ( T* dst, const T* src, size_t nelements)
  {
      memcpy(dst,src,nelements*sizeof(T));
  }
  template <typename T> void move ( T* dst, const T* src, size_t nelements)
  {
      memmove(dst,src,nelements*sizeof(T));
  }

  /** buffer  - in-memory dynamic buffer implementation. **/
  template <typename T>
    class buffer
    {
      T*              _body;
      size_t          _allocated;
      size_t          _size;

      T*  reserve(size_t size)
      {
        size_t newsize = _size + size;
        if( newsize > _allocated )
        {
          _allocated = (_allocated * 3) / 2;
          if(_allocated < newsize) _allocated = newsize;
          T *newbody = new T[_allocated];
          copy(newbody,_body,_size);
          delete[] _body;
          _body = newbody;
        }
        return _body + _size;
      }

    public:

      buffer():_size(0)        { _body = new T[_allocated = 256]; }
      ~buffer()                { delete[] _body;  }

      const T * data() const
      {
         buffer* self = const_cast<buffer*>(this);
         if(_size == _allocated) self->reserve(1);
         self->_body[_size] = 0; return _body;
      }

      size_t length() const         { return _size; }

      void push(T c)                { *reserve(1) = c; ++_size; }
      void push(const T *pc, size_t sz) { copy(reserve(sz),pc,sz); _size += sz; }

      void clear()                  { _size = 0; }

    };

    typedef buffer<BYTE> byte_buffer;
    typedef buffer<WCHAR> wchar_buffer;
    typedef buffer<char> char_buffer;
}

namespace utf8
{
  // convert utf8 code unit sequence to WCHAR sequence

  inline bool towcs(const BYTE *utf8, size_t length, pod::wchar_buffer& outbuf)
  {
    if(!utf8 || length == 0) return true;
    const BYTE* pc = (const BYTE*)utf8;
    const BYTE* last = pc + length;
    unsigned int b;
    unsigned int num_errors = 0;
    while (pc < last)
    {
      b = *pc++;

      if( !b ) break; // 0 - is eos in all utf encodings

      if ((b & 0x80) == 0)
      {
        // 1-BYTE sequence: 000000000xxxxxxx = 0xxxxxxx
        ;
      }
      else if ((b & 0xe0) == 0xc0)
      {
        // 2-BYTE sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
        if(pc == last) { outbuf.push('?'); ++num_errors; break; }
        b = (b & 0x1f) << 6;
        b |= (*pc++ & 0x3f);
      }
      else if ((b & 0xf0) == 0xe0)
      {
        // 3-BYTE sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
        if(pc >= last - 1) { outbuf.push('?'); ++num_errors; break; }

        b = (b & 0x0f) << 12;
        b |= (*pc++ & 0x3f) << 6;
        b |= (*pc++ & 0x3f);
        if(b == 0xFEFF &&
           outbuf.length() == 0) // bom at start
             continue; // skip it
      }
      else if ((b & 0xf8) == 0xf0)
      {
        // 4-BYTE sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
        if(pc >= last - 2) { outbuf.push('?'); break; }

        b = (b & 0x07) << 18;
        b |= (*pc++ & 0x3f) << 12;
        b |= (*pc++ & 0x3f) << 6;
        b |= (*pc++ & 0x3f);
        // b shall contain now full 21-bit unicode code point.
        assert((b & 0x1fffff) == b);
        if((b & 0x1fffff) != b)
        {
          outbuf.push('?');
          ++num_errors;
          continue;
        }
        outbuf.push( WCHAR(0xd7c0 + (b >> 10)) );
        outbuf.push( WCHAR(0xdc00 | (b & 0x3ff)) );
        continue;
      }
      else
      {
        assert(0); //bad start of UTF-8 multi-BYTE sequence"
        ++num_errors;
        b = '?';
      }
      outbuf.push( WCHAR(b) );
    }
    return num_errors == 0;
  }

  // gets full unicode code point from UTF16 sequence
  inline bool get_ucp(aux::wchars &buf, unsigned int& ucp)
  {
    if (buf.length == 0)
      return false;
    WCHAR c = *buf.start;
    buf.prune(1);
    if (c < 0xD800 || c > 0xDBFF) {
      ucp = c;
      return true; // not a surrogate pair
    }
    if (buf.length == 0) {
      assert(false); // surrogate pair is not complete
      ucp = c;
      return false;
    }
    WCHAR nc = *buf.start;
    buf.prune(1);
    ucp = (c - 0xD800) * 0x400 + (nc - 0xDC00) + 0x10000;
    return true;
  }

  // gets full unicode code point from UTF16 sequence
  inline bool get_ucp(const WCHAR* &pc, unsigned int& ucp)
  {
    if (*pc == 0)
      return false;
    WCHAR c = *pc++;
    if (c < 0xD800 || c > 0xDBFF) {
      ucp = c;
      return true; // not a surrogate pair
    }
    if (*pc == 0) {
      assert(false); // surrogate pair is not complete
      ucp = 0;
      return false;
    }
    WCHAR nc = *pc++;
    ucp = (c - 0xD800) * 0x400 + (nc - 0xDC00) + 0x10000;
    return true;
  }
  
  inline bool fromwcs(aux::wchars buf, pod::byte_buffer& outbuf)
  {
    unsigned int  num_errors = 0;
    unsigned int  c; // unicode code point

    while(get_ucp(buf,c))
    {
      if (c < (1 << 7))
      {
        outbuf.push(BYTE(c));
      }
      else if (c < (1 << 11))
      {
        outbuf.push(BYTE((c >> 6) | 0xc0));
        outbuf.push(BYTE((c & 0x3f) | 0x80));
      }
      else if (c < (1 << 16))
      {
        outbuf.push(BYTE((c >> 12) | 0xe0));
        outbuf.push(BYTE(((c >> 6) & 0x3f) | 0x80));
        outbuf.push(BYTE((c & 0x3f) | 0x80));
      }
      else if (c < (1 << 21))
      {
        outbuf.push(BYTE((c >> 18) | 0xf0));
        outbuf.push(BYTE(((c >> 12) & 0x3f) | 0x80));
        outbuf.push(BYTE(((c >> 6) & 0x3f) | 0x80));
        outbuf.push(BYTE((c & 0x3f) | 0x80));
      }
      else
        ++num_errors;
    }
    return num_errors == 0;
  }

  // UTF8 stream

  // class T must have two methods:
  //   void push(unsigned char c)
  //   void push(const unsigned char *pc, size_t sz)

  // bool X - true - XML markup character conversion (characters '<','>',etc).
  //          false - no conversion at all.

  template <class T, bool X = true>
  class ostream_t : public T
  {
  public:
    ostream_t()
    {
      // utf8 BYTE order mark
      static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
      T::push(BOM, sizeof(BOM));
    }

    // intended to handle only ascii-7 strings
    // use this for markup output
    ostream_t& operator << (const char* str)
    {
      T::push((const unsigned char*)str,strlen(str)); return *this;
    }

    ostream_t& operator << (char c)
    {
      T::push((unsigned char)c); return *this;
    }

    // use UNICODE chars for value output
    ostream_t& operator << (const WCHAR* wstr)
    {
      const WCHAR* p = wstr;
      unsigned int c;
      while(get_ucp(p,c))
      {
        if(X)
          switch(c)
          {
              case '<': *this << "&lt;"; continue;
              case '>': *this << "&gt;"; continue;
              case '&': *this << "&amp;"; continue;
              case '"': *this << "&quot;"; continue;
              case '\'': *this << "&apos;"; continue;
          }
        if (c < (1 << 7))
        {
         T::push (BYTE(c));
        }
        else if (c < (1 << 11)) {
         T::push (BYTE((c >> 6) | 0xc0));
         T::push (BYTE((c & 0x3f) | 0x80));
        }
        else if (c < (1 << 16)) {
         T::push (BYTE((c >> 12) | 0xe0));
         T::push (BYTE(((c >> 6) & 0x3f) | 0x80));
         T::push (BYTE((c & 0x3f) | 0x80));
        }
        else if (c < (1 << 21))
        {
         T::push (BYTE((c >> 18) | 0xf0));
         T::push (BYTE(((c >> 12) & 0x3f) | 0x80));
         T::push (BYTE(((c >> 6) & 0x3f) | 0x80));
         T::push (BYTE((c & 0x3f) | 0x80));
        }
      }
      return *this;
    }
    ostream_t& operator << (const std::wstring& str)
    {
      return *this << (str.c_str());
    }

  };

  // raw ASCII/UNICODE -> UTF8 converter
  typedef ostream_t<pod::byte_buffer,false> ostream;
  // ASCII/UNICODE -> UTF8 converter with XML support
  typedef ostream_t<pod::byte_buffer,true> oxstream;


} // namespace utf8

namespace aux
{
  template <typename T> struct slice;

  template <class T>
  inline T
    limit ( T v, T minv, T maxv )
  {
    assert(minv < maxv);
    if (minv >= maxv)
      return minv;
    if (v > maxv) return maxv;
    if (v < minv) return minv;
    return v;
  }

  // safe string comparison
  inline bool streq(const char* s, const char* s1)
  {
    if( s && s1 )
      return strcmp(s,s1) == 0;
    return false;
  }

  // safe wide string comparison
/*  inline bool wcseq(const WCHAR* s, const WCHAR* s1)
  {
    if( s && s1 )
       return std::wcscmp(s,s1) == 0;
    return false;
  } */

  // safe case independent string comparison
  inline bool streqi(const char* s, const char* s1)
  {
    if( s && s1 )

      return stricmp(s,s1) == 0;
    return false;
  }

  // safe case independent wide string comparison
/*  inline bool wcseqi(const WCHAR* s, const WCHAR* s1)
  {
    if( s && s1 )
      return wcsicmp(s,s1) == 0;
    return false;
  } */

  // helper convertor objects WCHAR to utf8 and vice versa
  class utf2w
  {
    pod::wchar_buffer buffer;
  public:
    explicit utf2w(const BYTE* utf8, size_t length = 0)
    {
      if(utf8)
      {
        if( length == 0) length = strlen((const char*)utf8);
        utf8::towcs(utf8, length ,buffer);
      }
    }
    explicit utf2w(const char* utf8, size_t length = 0)
    {
      if(utf8)
      {
        if( length == 0) length = strlen(utf8);
        utf8::towcs((const BYTE*)utf8, length ,buffer);
      }
    }
    explicit utf2w(const std::string& utf8)
    {
      utf8::towcs((const BYTE*)utf8.c_str(), utf8.length() ,buffer);
    }

    ~utf2w() {}

    operator const WCHAR*() const { return buffer.data(); }
    const WCHAR*  c_str() const { return buffer.data(); }
    size_t length() const { return buffer.length(); }

    pod::wchar_buffer& get_buffer() { return buffer; }

    operator aux::wchars() const { return aux::wchars(buffer.data(),buffer.length()); }
    aux::wchars chars() const { return aux::wchars(buffer.data(),buffer.length()); }
    //operator std::basic_string<WCHAR>() const { return std::basic_string<WCHAR>(buffer.data(),buffer.length()); }

  };

  class w2utf
  {
    pod::byte_buffer buffer;
  public:
    explicit w2utf(const WCHAR* wstr)
    {
      if(wstr)
      {
        utf8::fromwcs(chars_of(wstr),buffer);
      }
    }
    explicit w2utf(const std::basic_string<WCHAR>& str)
    {
      utf8::fromwcs(chars_of(str),buffer);
    }
    explicit w2utf(wchars str)
    {
      utf8::fromwcs(str, buffer);
    }
    ~w2utf() {}
    operator const BYTE*() const { return buffer.data(); }
    operator const char*() const { return (const char*)buffer.data(); }
    //operator std::string() const { return std::string(c_str(),length()); }

    const char*  c_str() const { return (const char*)buffer.data(); }
    chars operator()() const { return chars(c_str(),length()); }

    size_t length() const { return buffer.length(); }
  };

#ifdef WINDOWS

  // helper convertor objects WCHAR to ANSI string and vice versa
  class w2a
  {
    char   local[16];
    char*  buffer;
    size_t n;

    void init(const WCHAR* wstr, size_t nu)
    {
      n = WideCharToMultiByte(CP_ACP,0,wstr,int(nu),0,0,0,0);
      buffer = (n < (16-1))? local:new char[n+1];
      WideCharToMultiByte(CP_ACP,0,wstr,int(nu),buffer,int(n),0,0);
      buffer[n] = 0;
    }
  public:
    explicit w2a(const WCHAR* wstr):buffer(0),n(0)
    {
      if(wstr)
        init(wstr,wcslen(wstr));
    }
    explicit w2a(const std::basic_string<WCHAR>& wstr):buffer(0),n(0)
    {
      init(wstr.c_str(),wstr.length());
    }
    explicit w2a(slice<WCHAR> s):buffer(0), n(0)
    {
       init(s.start,s.length);
    }
    ~w2a() { if(buffer != local) delete[] buffer;  }
    size_t length() const { return n; }
    operator const char*() { return buffer; }
    const char*  c_str() const { return buffer; }
  };

  class a2w
  {
    WCHAR  local[16];
    WCHAR* buffer;
    size_t   nu;
    void init(const char* str, size_t n)
    {
#ifdef _WIN32_WCE
      nu = MultiByteToWideChar(CP_ACP,0,str,n,0,0);
#else
      nu = MultiByteToWideChar(CP_THREAD_ACP,0,str,int(n),0,0);
#endif
      buffer = ( nu < (16-1) )? local: new WCHAR[nu+1];
#ifdef _WIN32_WCE
      MultiByteToWideChar(CP_ACP,0,str,n,buffer,nu);
#else
      MultiByteToWideChar(CP_THREAD_ACP,0,str,int(n),buffer,int(nu));
#endif
      buffer[nu] = 0;
    }
  public:
    explicit a2w(const char* str):buffer(0), nu(0)
    {
      if(str)
        init(str, strlen(str));
    }
    explicit a2w(slice<char> s):buffer(0), nu(0)
    {
      init(s.start,s.length);
    }
    explicit a2w(const std::string& s):buffer(0), nu(0)
    {
      init(s.c_str(),s.length());
    }

    ~a2w() {  if(buffer != local) delete[] buffer;  }
    size_t length() const { return nu; }
    operator const WCHAR*() const { return buffer; }
    const WCHAR*  c_str() const { return buffer; }
    operator aux::wchars() const { return aux::wchars(buffer,nu); }
    aux::wchars chars() const { return aux::wchars(buffer,nu); }
    //operator std::basic_string<WCHAR>() const { return std::basic_string<WCHAR>(buffer,nu); }

  };

  class w2oem
  {
    char   local[64];
    char*  buffer;
    size_t n;

    void init(const WCHAR* wstr, size_t nu)
    {
      n = WideCharToMultiByte(CP_OEMCP,0,wstr,int(nu),0,0,0,0);
      buffer = (n < (64-1))? local : new char[n+1];
      WideCharToMultiByte(CP_OEMCP,0,wstr,int(nu),buffer,int(n),0,0);
      buffer[n] = 0;
    }

    w2oem(const w2oem &);
    w2oem& operator=(const w2oem &);

  public:
    explicit w2oem(const WCHAR* wstr):buffer(0),n(0)
    {
      local[0] = 0;
      if(wstr)
        init(wstr,wcslen(wstr));
    }
    explicit w2oem(const WCHAR* wstr, size_t sz) :buffer(0), n(0)
    {
      local[0] = 0;
      if (wstr)
        init(wstr, sz);
    }
    explicit w2oem(const std::wstring& wstr):buffer(0),n(0)
    {
      local[0] = 0;
      init(wstr.c_str(),wstr.length());
    }
    explicit w2oem(slice<WCHAR> s);

    ~w2oem() { if(buffer != local) delete[] buffer;  }
    size_t length() const { return n; }
    operator const char*() { return buffer; }
    const char*  c_str() const { return buffer; }
  };

#elif defined(UTF8_CHARS)
  typedef utf2w a2w;
  typedef w2utf w2a;
  typedef utf2w oem2w;
  typedef w2utf w2oem;
#else
  #error "unknown char string representation"
#endif

  /** Integer to string converter.
      Use it as wostream << itow(234)
  **/

template<typename CT>
  class itot
  {
    CT buffer[38];
  public:
    itot(int n, int radix = 10)
    {
      static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	  	CT* wstr = buffer;
    	if (radix < 2 || radix > 35) { *wstr = 0; return; }   // validate radix
		  int sign = n;	if (sign < 0) n = -n;
		  do { *wstr++ = num[n % radix]; } while( n /= radix ); // conversion. number is reversed.
	    if(sign<0) *wstr++ ='-';
	    *wstr = 0;
	    // reverse the string
      std::reverse(buffer,wstr);
    }
    operator const CT*() { return buffer; }
  };

  typedef itot<char> itoa;
  typedef itot<WCHAR> itow;


  /** Float to string converter.
      Use it as ostream << ftoa(234.1); or
      Use it as ostream << ftoa(234.1,"pt"); or
  **/
  class ftoa
  {
    char buffer[64];
  public:
    ftoa(double d, const char* units = "", int fractional_digits = 1)
    {
      snprintf(buffer, sizeof(buffer), "%.*f%s", fractional_digits, d, units );
      buffer[63] = 0;
    }
    operator const char*() { return buffer; }
  };

  /** Float to wstring converter.
      Use it as wostream << ftow(234.1); or
      Use it as wostream << ftow(234.1,"pt"); or
  **/
  /*class ftow
  {
    WCHAR buffer[64];
  public:
    ftow(double d, const WCHAR* units = "", int fractional_digits = 1)
    {
#ifdef WINDOWS
      _snwprintf_s(buffer, 64, L"%.*f%s", fractional_digits, d, units );
#else
      swprintf(buffer, 64, L"%.*f%s", fractional_digits, d, units );
#endif
      buffer[63] = 0;
    }
    operator const WCHAR*() { return buffer; }
  };*/

  /** string to integer parser. **/
   inline int atoi(const char *s, int default_value = 0)
  {
    if( !s ) return default_value;
    char *lastptr;
    long i = strtol( s, &lastptr, 10 );
    return (lastptr != s)? (int)i : default_value;
  }

 /** wstring to integer parser. **/
  inline int wtoi(const WCHAR *s, int default_value = 0)
  {
    w2a at(s);
    return atoi(at.c_str(),default_value);
  }

  // class T must have two methods:
  //   void push(WCHAR c)
  //   void push(const WCHAR *pc, size_t sz)
  template <class T>
  class ostream_t : public T
  {
  public:
    ostream_t() {}

    // intended to handle only ascii-7 strings
    // use this for markup output
    ostream_t& operator << (const char* str)
    {
      if(!str) return *this;
      while( *str ) T::push(*str++);
      return *this;
    }

    ostream_t& operator << (char c)
    {
      T::push(c); return *this;
    }

    // intended to handle only ascii-7 strings
    // use this for markup output
    ostream_t& operator << (const WCHAR* str)
    {
      if(!str || !str[0]) return *this;
      T::push(str,wcslen(str)); return *this;
    }

    ostream_t& operator << (WCHAR c)
    {
      T::push(c); return *this;
    }
  };

  // wostream - a.k.a. wstring builder - buffer for dynamic composition of WCHAR strings
  typedef ostream_t<pod::wchar_buffer> wostream;
}

#endif
#endif
