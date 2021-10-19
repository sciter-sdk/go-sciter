#ifndef __aux_slice_h__
#define __aux_slice_h__

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

/*
 * Terra Informatica Sciter and HTMLayout Engines
 * http://terrainformatica.com/sciter, http://terrainformatica.com/htmlayout
 *
 * slice - range of elements, start/length. That is what is known in D as array.
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief range of elements
 **/

#include <assert.h>
#include "limits.h"
#include <vector>
#include "sciter-x-primitives.h"

namespace aux
{

  inline bool is_space( char c ) { return isspace(c & 0xff) != 0; }
  inline bool is_space( WCHAR c ) { return iswspace(c) != 0; }

  inline bool is_digit( char c ) { return isdigit(c & 0xff) != 0; }
  inline bool is_digit( WCHAR c ) { return iswdigit(c) != 0; }

  inline bool is_xdigit( char c ) { return isdigit(c & 0xff) != 0; }
  inline bool is_xdigit( WCHAR c ) { return iswxdigit(c) != 0; }

  inline bool is_alpha( char c ) { return isalpha(c & 0xff) != 0; }
  inline bool is_alpha( WCHAR c ) { return iswalpha(c) != 0; }

  inline bool is_alnum( char c ) { return isalnum(c & 0xff) != 0; }
  inline bool is_alnum( WCHAR c ) { return iswalnum(c) != 0; }

  template<typename CT>
    inline size_t wcslen( const CT* s ) { const CT *p = s; while (*p) p++; return p - s; }

template <typename T >
   struct slice
   {
      const T*  start;
      size_t    length;

      slice(): start(0), length(0) {}
      slice(const T* start_, size_t length_) { start = start_; length = length_; }
      slice(const slice& src): start(src.start), length(src.length) {}

      slice& operator = (const slice& src) { start = src.start; length = src.length; return *this; }

      // definitions to support for(auto a : slice) {} loops in C++11
      const T* begin() const { return start; }
      const T* end() const { return start + length; }

      bool operator == ( const slice& r ) const
      {
        if( length != r.length )
          return false;
        if( start == r.start )
          return true;
        for( unsigned int i = 0; i < length; ++i )
          if( start[i] != r.start[i] )
            return false;
        return true;
      }

      bool operator != ( const slice& r ) const { return !operator==(r); }

      T operator[] ( unsigned int idx ) const
      {
        assert( idx < length );
        if(idx < length)
          return start[idx];
        return 0;
      }

      T last() const
      {
        if(length)
          return start[length-1];
        return 0;
      }

      // [idx1..length)
      slice operator() ( unsigned int idx1 ) const
      {
        assert( idx1 < length );
        if ( idx1 < length )
            return slice( start + idx1, length - idx1 );
        return slice();
      }
      // [idx1..idx2)
      slice operator() ( unsigned int idx1, unsigned int idx2 ) const
      {
        assert( idx1 < length );
        assert( idx2 <= length );
        assert( idx1 <= idx2 );
        if ( idx1 < idx2 )
            return slice( start + idx1, idx2 - idx1 );
        return slice();
      }

      int index_of( T e ) const
      {
        for( unsigned int i = 0; i < length; ++i ) if( start[i] == e ) return i;
        return -1;
      }

      int last_index_of( T e ) const
      {
        for( size_t i = length; i > 0 ;) if( start[--i] == e ) return int(i);
        return -1;
      }

      int index_of( const slice& s ) const
      {
        if( s.length > length ) return -1;
        if( s.length == 0 ) return -1;
        size_t l = length - s.length;
        for( unsigned int i = 0; i < l ; ++i)
          if( start[i] == *s.start )
          {
            const T* p = s.start;
            size_t last = i + s.length;
            for( unsigned int j = i + 1; j < last; ++j )
              if( *(++p) != start[j])
                goto next_i;
            return i;
            next_i: continue;
          }
        return -1;
      }

      int last_index_of( const slice& s ) const
      {
        if( s.length > length ) return -1;
        if( s.length == 0 ) return -1;
        const T* ps = s.end() - 1;
        for( size_t i = length; i > 0 ; )
          if( start[--i] == *ps )
          {
            const T* p = ps;
            size_t j, first = i - s.length + 1;
            for( j = i; j > first; )
              if( *(--p) != start[--j])
                goto next_i;
            return int(j);
            next_i: continue;
          }
        return -1;
      }

      template <class Y>
      bool starts_with(const slice<Y> &s) const {
        if (length < s.length)
          return false;
        slice t(start, s.length);
        return t == s;
      }

      bool ends_with(const slice &s) const {
        if (length < s.length)
          return false;
        slice t(start + length - s.length, s.length);
        return t == s;
      }

      void prune(size_t from_start, size_t from_end = 0)
      {
        size_t s = from_start >= length? length : from_start;
        size_t e = length - (from_end >= length? length: from_end);
        start += s;
        if( s < e ) length = e-s;
        else length = 0;
      }

      bool split(const slice &delimeter, slice &head, slice &tail) const {
        int d = index_of(delimeter);
        if (d < 0)
          return false;
        const T *s = start;
        size_t   l = length;
        head = slice(s, d);
        tail = slice(s + d + delimeter.length, l - d - delimeter.length);
        return true;
      }


#ifdef CPP11
      explicit
#endif
      operator bool() const { return length > 0; }

      T operator *() const { return length ? start[0] : T(); }

      T operator++() { if (length) { ++start; if (--length) return *start; } return T(); }
      T operator++(int) { if (length) { --length; ++start; return *(start - 1); } return T(); }

      bool like(const T* pattern) const;

   };

  #define MAKE_SLICE( T, D ) slice<T>(D, sizeof(D) / sizeof(D[0]))

  // gets (static) array of elements and constructs slice of it (start/length pair)
  template<typename T, int size>
    inline slice<T> make_slice(const T(&arr)[size]) { return slice<T>(&arr[0], size); }

  #ifdef _DEBUG

  inline void slice_unittest()
  {
    int v1[] = { 0,1,2,3,4,5,6,7,8,9 };
    int v2[] = { 3,4,5 };
    int v3[] = { 0,1,2 };
    int v4[] = { 0,1,2,4 };
    int v5[] = { 1,1,2,3 };

    slice<int> s1 = MAKE_SLICE( int, v1 );
    slice<int> s2 = MAKE_SLICE( int, v2 );
    slice<int> s3 = MAKE_SLICE( int, v3 );
    slice<int> s4 = MAKE_SLICE( int, v4 );
    slice<int> s5 = MAKE_SLICE( int, v5 );

    assert( s1 != s2 );
    assert( s1(3,6) == s2 );
    assert( s1.index_of(3) == 3 );
    assert( s1.index_of(s2) == 3 );
    assert( s1.last_index_of(3) == 3 );
    assert( s1.last_index_of(s2) == 3 );

    assert( s1.index_of(s3) == 0 );
    assert( s1.last_index_of(s3) == 0 );

    assert( s1.index_of(s4) == -1 );
    assert( s1.last_index_of(s4) == -1 );

    assert( s1.index_of(s5) == -1 );
    assert( s1.last_index_of(s5) == -1 );

  }

  #endif

  template <typename CS>
    inline slice<CS> trim_left(slice<CS> str)
  {
    for( unsigned i = 0; i < str.length; ++i )
      if( is_space(str[0]) ) { ++str.start; --str.length; }
      else break;
    return str;
  }

  template <typename CS>
    inline slice<CS> trim_right(slice<CS> str)
  {
    for( int j = int(str.length) - 1; j >= 0; --j )
      if( is_space(str[j]) ) --str.length;
      else break;
    return str;
  }

  template <typename CS>
    inline slice<CS> trim(slice<CS> str)
  {
    return trim_right(trim_left(str));
  }

  typedef slice<char>    chars;
  typedef slice<WCHAR>   wchars;
  typedef slice<BYTE>    bytes;


  // Note: CS here is a string literal!
  #define const_chars(CS) aux::slice<char>(CS,CHARS_IN(CS))
  #define const_wchars(CS) aux::slice<WCHAR>(WSTR(CS),CHARS_IN(_WSTR(CS)))

  inline wchars  chars_of( const WCHAR *t )   {  return t? wchars(t,(unsigned int)wcslen(t)):wchars(); }
  inline chars   chars_of( const char *t )    {  return t? chars(t,(unsigned int)strlen(t)):chars(); }

  

  template<typename T>
     slice<T> chars_of( const std::basic_string<T> &s ) {  return slice<T>(s.c_str(), s.length()); }

  template<typename T>
     slice<T> elements_of(const T* s, size_t l) { return slice<T>(s, l); }

  template<typename T>
     slice<T> elements_of( const std::vector<T> &s ) {  return slice<T>(s.data(), s.size()); }

  template<typename T, int size>
     slice<T> elements_of(T (& arr)[size]){ return slice<T>(&arr[0],size);}

  template<typename T, int size>
     slice<T> elements_of(const T (& arr)[size]){ return slice<T>(&arr[0],size);}

  template<typename T>
     std::basic_string<T> make_string( aux::slice<T> s ) { return std::basic_string<T>(s.start, s.length); }

   // simple tokenizer
  template <typename T >
      class tokens
      {
        const T* delimeters;
        const T* p;
        const T* tail;
        const T* start;
        const T* end;
        const bool  is_delimeter(T el)  { for(const T* t = delimeters;t && *t; ++t) if(el == *t) return true;  return false; }
        const T*    tok()               { for(;p < tail; ++p) if(is_delimeter(*p)) return p++; return p; }
      public:

        tokens(const T *text, size_t text_length, const T* separators): delimeters(separators)
        {
          start = p = text;
          tail = p + text_length;
          end = tok();
        }

        tokens(const aux::slice<T> s, const T* separators): delimeters(separators)
        {
          start = p = s.start;
          tail = p + s.length;
          end = tok();
        }

        bool next(slice<T>& v)
        {
          if(start < tail)
          {
            v.start = start;
            v.length = unsigned(end - start);
            start = p;
            end = tok();
            return true;
          }
          return false;
        }
      };

  typedef tokens<char> atokens;
  typedef tokens<wchar_t> wtokens;


    /****************************************************************************/
    //
    // idea was taken from Konstantin Knizhnik's FastDB
    // see http://www.garret.ru/
    // extended by [] operations
    //

  template <typename CT, CT sep = '-', CT end = ']' >
    struct charset
    {
      #define SET_SIZE (1 << (sizeof(CT) * CHAR_BIT))
      #define SIGNIFICANT_BITS_MASK unsigned( SET_SIZE - 1 )

      unsigned char codes[ SET_SIZE / CHAR_BIT ];

      unsigned charcode(CT c) // proper unsigned_cast
      {
        return SIGNIFICANT_BITS_MASK & unsigned(c);
      }

    private:
      void set ( CT from, CT to, bool v )
      {
         for ( unsigned i = charcode(from); i <= charcode(to); ++i )
         {
           unsigned int bit = i & 7;
           unsigned int octet = i >> 3;
           if( v ) codes[octet] |= 1 << bit; else codes[octet] &= ~(1 << bit);
         }
      }
      void init ( unsigned char v )  { memset(codes,v,(SET_SIZE >> 3)); }
    public:

      void parse ( const CT* &pp )
      {
        //assert( sizeof(codes) == sizeof(CT) * sizeof(bool));
        const CT *p = (const CT *) pp;
        unsigned char inv = *p == '^'? 0xff:0;
        if ( inv ) { ++p; }
        init ( inv );
        if ( *p == sep ) set(sep,sep,inv == 0);
        while ( *p )
        {
          if ( p[0] == end ) { p++; break; }
          if ( p[1] == sep && p[2] != 0 ) { set (p[0], p[2], inv == 0 );  p += 3; }
          else { CT t = *p++; set(t,t, inv == 0); }
        }
        pp = (const CT *) p;
      }

      bool valid ( CT c )
      {
        unsigned int bit = charcode(c) & 7;
        unsigned int octet = charcode(c) >> 3;
        return (codes[octet] & (1 << bit)) != 0;
      }
      #undef SET_SIZE
      #undef SIGNIFICANT_BITS_MASK
    };

  template <typename CT >
    inline int match ( slice<CT> cr, const CT *pattern )
    {
      const CT AnySubstring = '*';
      const CT AnyOneChar = '?';
      const CT AnyOneDigit = '#';

      const CT    *str = cr.start;
      const CT    *wildcard = 0;
      const CT    *strpos = 0;
      const CT    *matchpos = 0;

      charset<CT> cset;

      for (;;)
      {
        if ( *pattern == AnySubstring )
        {
          wildcard = ++pattern;
          strpos = str;
          if ( !matchpos ) matchpos = str;
        }
        else if ( *str == '\0' || str >= cr.end() )
        {
          return ( *pattern == '\0' ) ? int( matchpos - cr.start ) : -1;
        }
        else if ( *pattern == '[' )
        {
          pattern++;
          cset.parse ( pattern );
          if ( !cset.valid ( *str ) )
            return -1;
          if ( !matchpos )
            matchpos = str;
          str += 1;
        }
        else if ( *str == *pattern || *pattern == AnyOneChar )
        {
          if ( !matchpos ) matchpos = str;
          str += 1;
          pattern += 1;
        }
        else if (*pattern == AnyOneDigit)
        {
          if (isdigit(*str)) {
            if (!matchpos) matchpos = str;
            str++;
            pattern++;
          }
          else if (wildcard) {
            str = ++strpos;
            pattern = wildcard;
          }
          else
            return -1;
        }
        else if ( wildcard )
        {
          str = ++strpos;
          pattern = wildcard;
        }
        else
          break;
      }
      return -1;
    }

  template <typename T >
    inline bool slice<T>::like ( const T *pattern ) const
    {
      return match<T>(*this,pattern) >= 0;
    }

  // chars to unsigned int
  // chars to int

  template <typename T>
      inline unsigned int to_uint(slice<T>& span, unsigned int base = 10)
  {
     unsigned int result = 0,value;
     const T *cp = span.start;
     const T *pend = span.end();

     while ( cp < pend && is_space(*cp) ) ++cp;

     if (!base)
     {
         base = 10;
         if (*cp == '0') {
             base = 8;
             cp++;
             if ((toupper(*cp) == 'X') && is_xdigit(cp[1])) {
                     cp++;
                     base = 16;
             }
         }
     }
     else if (base == 16)
     {
         if (cp[0] == '0' && toupper(cp[1]) == 'X')
             cp += 2;
     }
     while ( cp < pend && is_xdigit(*cp) &&
            (value = is_digit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base) {
             result = result*base + value;
             cp++;
     }
     span.length = (unsigned int)(cp - span.start);
     return result;
  }

  template <typename T>
      int to_int(slice<T>& span, unsigned int base = 10)
  {

     while (span.length > 0 && is_space(span[0]) ) { ++span.start; --span.length; }
     if(span[0] == '-')
     {
        ++span.start; --span.length;
        return - int(to_uint(span,base));
     }
     return to_uint(span,base);
  }

}

#endif
#endif
