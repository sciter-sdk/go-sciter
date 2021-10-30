/** \mainpage Terra Informatica Sciter engine.
 *
 * \section legal_sec In legalese
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * <a href="http://terrainformatica.com/Sciter">Sciter Home</a>
 *
 * (C) 2003-2014, Terra Informatica Software, Inc. and Andrew Fedoniouk
 *
 **/

/**\file
 * \brief value.hpp VALUE C++ wrapper
 **/

#ifndef __value_hpp__
#define __value_hpp__

  #include "value.h"

  #include <string>
  #include <functional>
  #include <stdexcept>
#ifdef CPP11
  #include <initializer_list>
  #include <utility>
  #include <type_traits>
#endif

  #include "aux-slice.h"
  #include "aux-cvt.h"
  #include "sciter-x-primitives.h"

#if defined(_MSC_VER) && (_MSC_VER < 1600) // MSVC version < 8
     #include "nullptr.hpp"
#endif

  #pragma warning( push )
  #pragma warning(disable:4786) //identifier was truncated...

  struct som_asset_t;
  struct som_asset_class_t;
  
  namespace sciter
  {

    typedef unsigned char byte;

    template<typename TC>
      inline size_t str_length(const TC* src ) { 
        if(!src) return 0;
        size_t cnt = 0; while( *src++ ) ++cnt;
        return cnt;
      }
    
    // wide (utf16) string
    typedef std::basic_string<WCHAR> string;
    // ascii or utf8 string
    typedef std::basic_string<char>  astring;

    typedef std::runtime_error script_error;

    // value by key bidirectional proxy/accessor 
    class value_key_a;
    // value by index bidirectional proxy/accessor 
    class value_idx_a;
    class value;

    // setter - free standing conversion of T to sciter::value.
    // NOTE this function is deliberatly left not implemented - it is just a prototype
    template<typename T>
      inline value setter(const T& v);


#ifdef CPP11
    // native function that can be stored inside the value:
    typedef std::function<value(unsigned int argc, const value* argv)> native_function_t;
#endif

    class value: public VALUE
    {
      value(void*) {} // no such thing, sorry
      //void* get(const void* defv) const { return 0; } // and this one too is disabled
      //void* get(const void* defv) { return 0; } // and this one too is disabled
    public:
      value()                 { ValueInit(this); }
     ~value()                 { ValueClear(this); }
      value(const value& src) { ValueInit(this); ValueCopy(this,&src); }
      value(const VALUE& src) { ValueInit(this); ValueCopy(this,&src); }

#ifdef CPP11
      value(value&& src) { ValueInit(this); std::swap( *(VALUE*)this, *(VALUE*)&src); }
#endif

      value(const value_key_a& src);
      value(const value_idx_a& src);
      
      value& operator = (const value& src) { ValueCopy(this,&src); return *this; }
      value& operator = (const VALUE& src) { ValueCopy(this,&src); return *this; }

      value( bool v )           { ValueInit(this); ValueIntDataSet(this, v?1:0, T_BOOL, 0); }
      value( int  v )           { ValueInit(this); ValueIntDataSet(this, v, T_INT, 0); }
      value( unsigned int v )   { ValueInit(this); ValueIntDataSet(this, int(v), T_INT, 0); }
      value( double v )         { ValueInit(this); ValueFloatDataSet(this, v, T_FLOAT, 0); }
      value( float v )          { ValueInit(this); ValueFloatDataSet(this, v, T_FLOAT, 0); }

      value( const WCHAR* s, unsigned int slen = 0 ) { ValueInit(this); ValueStringDataSet(this, LPCWSTR(s), (slen || !s)? slen : (unsigned int)str_length(s), 0); }
      value( const string& s ) { ValueInit(this); ValueStringDataSet(this, LPCWSTR(s.c_str()), UINT(s.length()), 0); }
      value( const astring& s ) { aux::utf2w as(s.c_str()); ValueInit(this); ValueStringDataSet(this, LPCWSTR(as.c_str()), UINT(as.length()), UT_STRING_SYMBOL); }
      value( aux::wchars ws )   { ValueInit(this); ValueStringDataSet(this, LPCWSTR(ws.start), UINT(ws.length), 0); }
      value( aux::bytes bs )    { ValueInit(this); ValueBinaryDataSet(this, bs.start, UINT(bs.length), T_BYTES, 0); }
      value( const std::vector<byte>& bs) { ValueInit(this); ValueBinaryDataSet(this, &bs[0], UINT(bs.size()), T_BYTES, 0); }

      value( const value* arr, unsigned n )  { ValueInit(this); for( unsigned i = 0; i < n; ++i ) set_item(int(i),arr[i]); }
   template<typename T>
      value(const std::vector<T>& vec) { ValueInit(this); for (unsigned i = 0; i < vec.size(); ++i) set_item(int(i), value(vec[i])); }
#ifdef CPP11
   template<typename T, size_t N>
      value(const std::array<T,N>& arr) { ValueInit(this); for (unsigned i = 0; i < N; ++i) set_item(int(i), value(arr[i])); }
      value( const native_function_t& nfr );
#endif

      template<typename T> value(const T& v) : value(setter(v)) {;}
          
      static value currency( INT64 v )  { value t; ValueInt64DataSet(&t, v, T_CURRENCY, 0); return t;}
      static value date( INT64 v, bool is_utc = true /* true if ft is UTC*/ )      { value t; ValueInt64DataSet(&t, v, T_DATE, is_utc);  return t;}
#ifdef WIN32
      static value date( FILETIME ft, bool is_utc = true /* true if ft is UTC*/ )  { value t; ValueInt64DataSet(&t, *((INT64*)&ft), T_DATE, is_utc); return t;} 
#endif
      static value symbol( aux::wchars wc ) { value t; ValueInit(&t); ValueStringDataSet(&t, LPCWSTR(wc.start), UINT(wc.length) , 0xFFFF); return t; }

      /** set color value, abgr - a << 24 | b << 16 | g << 8 | r, where a,b,g,r are bytes */
      static value color(UINT abgr) { value t; ValueInit(&t); ValueIntDataSet(&t, abgr, T_COLOR, 0); return t; }
      
      /** set duration value, seconds */
      static value duration(double seconds) { value t; ValueInit(&t); ValueFloatDataSet(&t, seconds, T_DURATION, 0); return t; }

      /** set angle value, radians */
      static value angle(double radians) { value t; ValueInit(&t); ValueFloatDataSet(&t, radians, T_ANGLE, 0); return t; }

      // string-symbol
      value( const char* s ) 
      { 
        aux::a2w as(s);
        ValueInit(this); ValueStringDataSet(this, LPCWSTR(as.c_str()), UINT(as.length()), UT_STRING_SYMBOL);
      }
            
      static value make_string( const char* s ) 
      { 
        aux::a2w as(s);
        return value(as.chars());
      }
      static value make_string( const WCHAR* s )
      { 
        return value( aux::chars_of(s) );
      }
      static value make_string(const WCHAR* s, size_t len)
      {
        return value(aux::wchars(s,len));
      }

      static value make_bytes(const byte* s, size_t len)
      {
        return value(aux::bytes(s, len));
      }


      /** Creates an array of values packaged into the value 
          Creates an empty array if called with length == 0 */
      static value make_array( unsigned int length = 0, const value* elements = nullptr )
      {
        value v;
        ValueIntDataSet(&v, INT(length), T_ARRAY, 0);
        if( elements ) 
          for( unsigned int i = 0; i < length; ++i ) 
           v.set_item(INT(i),elements[i]);
        return v;
      }

#ifdef CPP11
      /** Create an array object initialized with the given values. */
      static value make_array(std::initializer_list<value> list)
      {
        return value::make_array(static_cast<unsigned>(list.size()), list.begin());
      }
#endif

      /** Creates an empty json key/value map (object in JS terms) 
          The map can be populated by map.set_item(key,val); */
      static value make_map(  )
      {
        value v;
        ValueIntDataSet(&v, INT(0), T_MAP, 0);
        return v;
      }
      
#ifdef CPP11
      /** Create a map object initialized with the given key/value pairs. */
      static value make_map(std::initializer_list<std::pair<value, value>> list)
      {
        value result = value::make_map();
        for (auto& item : list)
        {
          result.set_item(item.first, item.second);
        }
        return result;
      }
#endif
      
      static value secure_string(const WCHAR* s, size_t slen)
      {
        value v;
        ValueStringDataSet(&v, LPCWSTR(s), UINT(slen), UT_STRING_SECURE);
        return v;
      }

      static value make_error(const WCHAR* s) // returns string representing error. 
                                              // if such value is used as a return value from native function
                                              // the script runtime will throw an error in script rather than returning that value.
      {
        value v;
        if( !s ) return v;
        ValueStringDataSet(&v, LPCWSTR(s), UINT(aux::wcslen(s)), UT_STRING_ERROR);
        return v;
      }

      static value make_error(const char* s) // returns string representing error. 
                                              // if such value is used as a return value from native function
                                              // the script runtime will throw an error in script rather than returning that value.
      {
        value v;
        if (!s) return v;
        aux::a2w ws(s);
        ValueStringDataSet(&v, ws.c_str(), UINT(ws.length()), UT_STRING_ERROR);
        return v;
      }

      bool is_undefined() const { return t == T_UNDEFINED; }
      bool is_bool() const { return t == T_BOOL; }
      bool is_int() const { return t == T_INT; }
      bool is_float() const { return t == T_FLOAT; }
      bool is_string() const { return t == T_STRING; }
      bool is_symbol() const { return t == T_STRING && u == UT_STRING_SYMBOL; }
      bool is_error_string() const { return t == T_STRING && u == UT_STRING_ERROR; }
      bool is_date() const { return t == T_DATE; }
      bool is_currency() const { return t == T_CURRENCY; }
      bool is_map() const { return t == T_MAP; }
      bool is_array() const { return t == T_ARRAY; }
      bool is_array_like() const { return t == T_ARRAY || (t == T_OBJECT && u == UT_OBJECT_ARRAY); }
      bool is_function() const { return t == T_FUNCTION; }
      bool is_bytes() const { return t == T_BYTES; }
      bool is_object() const { return t == T_OBJECT; }
      //bool is_dom_element() const { return t == T_DOM_OBJECT; }
      // if it is a native functor reference
      bool is_native_function() const { return !!ValueIsNativeFunctor(this); }

      bool is_asset() const { return t == T_ASSET; }

      bool is_color() const { return t == T_COLOR; }
      bool is_duration() const { return t == T_DURATION; }
      bool is_angle() const { return t == T_ANGLE; }
      
      bool is_null() const { return t == T_NULL && u == 0; }
      static value null() { value n; n.t = T_NULL; return n; }

      bool is_nothing() const { return t == T_UNDEFINED && u == UT_NOTHING; }
      static value nothing() { value n; n.t = T_UNDEFINED; n.u = UT_NOTHING; return n; }

      bool operator == (const value& rs) const 
      {
        if( this == &rs ) return true;
        int r = ValueCompare( this, &rs );
        if( r == HV_OK )
          return false;
        else if (r == HV_OK_TRUE)
          return true;
        else
          assert(false);
        return false;
      }
      bool operator != (const value& rs) const 
      {
        return !(operator==(rs));
      }


      int get(int defv) const 
      {
        int v;
        if(ValueIntData(this,&v) == HV_OK) return v; 
        return defv;
      }
      double get(double defv) const 
      {
        double v;
        if(ValueFloatData(this,&v) == HV_OK) return v; 
        return defv;
      }
      string get(const WCHAR* defv) const
      {
        LPCWSTR c; UINT l;
        if(ValueStringData(this, &c,&l) == HV_OK)
          return aux::make_string(aux::wchars(c,l));
        return string(defv);
      }
      aux::wchars get_chars() const
      {
        LPCWSTR c; UINT l;
        if (ValueStringData(this, &c, &l) == HV_OK)
          return aux::wchars(c, l);
        return aux::wchars();
      }
      aux::bytes get_bytes() const 
      {
        LPCBYTE b; UINT l;
        ValueBinaryData(this,&b,&l);
        return aux::bytes(b,l);
      }

      UINT get_color(UINT defv = 0) const 
      {
        UINT v = defv;
        assert(is_color());
        ValueIntData(this, (INT*)&v);
        return v;
      }

      // returns radians if this->is_angle()
      double get_angle(double defv = 0) const 
      {
        double v = defv;
        assert(is_angle());
        ValueFloatData(this, &v);
        return v;
      }

      // returns seconds if this->is_duration()
      double get_duration(double defv = 0) const
      {
        double v = defv;
        assert(is_duration());
        ValueFloatData(this, &v);
        return v;
      }

#ifdef WIN32
      FILETIME get_date() const
      { 
        INT64 v;
        if(ValueInt64Data(this,&v) == HV_OK) return *((FILETIME*)&v); 
        return FILETIME();
      }
#endif

      som_asset_t* get_asset() const {
        if (!is_asset()) return nullptr;
        INT64 v;
        if (ValueInt64Data(this, &v) != HV_OK) return nullptr;
        return reinterpret_cast<som_asset_t*>(v);
      }

      // sqlite::Recordset* prs = val.get_asset<sqlite::Recordset>()
      template <class AT> AT* get_asset() const;

      static value wrap_asset(som_asset_t* pass) {
        value r;
        ValueInt64DataSet(&r, (UINT64)pass, T_ASSET, 0);
        return r;
      }

      bool get(bool defv) const 
      {
        int v;
        if(ValueIntData(this,&v) == HV_OK) return v != 0; 
        return defv;
      }

      template<class T>
      using UT = std::remove_const_t<std::remove_reference_t<T>>;

      template<typename T> UT<T> get() const { 
        return getter(*this,static_cast<UT<T>*>(nullptr));
      }      

      static value from_string(const WCHAR* s, size_t len = 0, VALUE_STRING_CVT_TYPE ct = CVT_SIMPLE)
      {
        value t;
        if( s ) 
        {
          if(len == 0) len = str_length(s);
          ValueFromString( &t, s, UINT(len), ct );
        }
        return t;
      }
      static value from_string(const std::basic_string<WCHAR>& s, VALUE_STRING_CVT_TYPE ct = CVT_SIMPLE)
      {
        return from_string(s.c_str(), (unsigned int)s.length(),ct);
      }
      static value from_string(aux::wchars s, VALUE_STRING_CVT_TYPE ct = CVT_SIMPLE)
      {
        return from_string(s.start, s.length,ct);
      }


      string to_string(int how = CVT_SIMPLE) const
      {
        if( how == CVT_SIMPLE && is_string() )
          return aux::make_string(get_chars()); // do not need to allocate
        value tv = *this;
        ValueToString(&tv,how);
        return aux::make_string(tv.get_chars());
      }

      void clear()
      {
        ValueClear(this);
      }

      // if it is an array or map returns number of elements there, otherwise - 0
      // if it is a function - returns number of arguments
      int length() const 
      {
        int n = 0;
        ValueElementsCount( this, &n);
        return n;
      }
      // if it is an array - returns nth element
      // if it is a map - returns nth value of the map
      // if it is a function - returns nth argument
      // otherwise it returns undefined value
      value get_item(int n) const
      {
        value r;
        ValueNthElementValue( this, n, &r);
        return r;
      }

      const value operator[](int n) const { return get_item(n); }
      value_idx_a operator[](int n);

      // if it is a map - returns value under the key in the map
      // if it is a function - returns value of argument with the name
      // otherwise it returns undefined value
      const value operator[](const value& key) const { return get_item(key); }
      value_key_a operator[](const value& key);

#ifdef CPP11
      typedef std::function<bool(const value& key, const value& val)> key_value_cb;
#endif

      struct enum_cb
      {
        // return true to continue enumeration
        virtual bool on(const value& key, const value& val) = 0;

        static SBOOL SC_CALLBACK _callback( LPVOID param, const VALUE* pkey, const VALUE* pval )
        {
          enum_cb* cb = (enum_cb*)param;
          return cb->on( *(value*)pkey, *(value*)pval );
        }
#ifdef CPP11
        static SBOOL SC_CALLBACK lambda_callback( LPVOID param, const VALUE* pkey, const VALUE* pval )
        {
          key_value_cb* cb = (key_value_cb*)param;
          return (*cb)(*(value*)pkey, *(value*)pval );
        }
#endif
      };

      // enum
      void enum_elements(enum_cb& cb) const
      {
        ValueEnumElements(const_cast<value*>(this), &enum_cb::_callback, &cb);
      }

#ifdef CPP11
      // calls cbf for each key/value pair found in T_OBJECT or T_MAP  
      void each_key_value(key_value_cb cbf) const
      {
        ValueEnumElements(const_cast<value*>(this), &enum_cb::lambda_callback, &cbf);
      }
#endif      
      
      value key(int n) const
      {
        value r;
        ValueNthElementKey( this, n, &r);
        return r;
      }

      // if it is an array - sets nth element expanding the array if needed
      // if it is a map - sets nth value of the map;
      // if it is a function - sets nth argument of the function;
      // otherwise it converts this to array and adds v as first element.
      void set_item(int n, const value& v)
      {
        ValueNthElementValueSet( this, n, &v);
      }

      void append(const value& v) 
      {
        ValueNthElementValueSet( this, length(), &v);
      }
      // if it is a map - sets named value in the map;
      // if it is a function - sets named argument of the function;
      // otherwise it converts this to map and adds key/v to it.

      void set_item(const value& key, const value& v)
      {
        ValueSetValueToKey( this,&key,&v );
      }
      void set_item(const char* name, const value& v)
      {
        value key(name);
        ValueSetValueToKey( this,&key,&v );
      }

      /** get value by key value
          \return \b #value under that key if this value is a map/object containing that key, otherwise undefined value */
      value get_item(const value& key) const
      {
        value r;
        ValueGetValueOfKey( this, &key, &r);
        return r;
      }

      /** get value by name 
          \return \b #value under that key if this value is a map/object containing that key, otherwise undefined value */
      value get_item(const char* name) const
      {
        value key(name);
        value r;
        ValueGetValueOfKey( this, &key, &r);
        return r;
      }
      
      // T_OBJECT only, get value of object's data slot
      void* get_object_data() const
      {
        LPCBYTE pv = 0; unsigned int dummy;
        UINT r = ValueBinaryData(this,&pv,&dummy); r = r;
        assert(r == HV_OK);
        return (void*)pv;
      }

      //
      // Below this point are TISCRIPT/SCITER related methods
      //

      bool is_object_native() const   {  return t == T_OBJECT && u == UT_OBJECT_NATIVE; }
      bool is_object_array() const    {  return t == T_OBJECT && u == UT_OBJECT_ARRAY; }
      bool is_object_function() const {  return t == T_OBJECT && u == UT_OBJECT_FUNCTION; }
      bool is_object_object() const   {  return t == T_OBJECT && u == UT_OBJECT_OBJECT; } // that is plain TS object
      bool is_object_class() const    {  return t == T_OBJECT && u == UT_OBJECT_CLASS; }  // that is TS class
      bool is_object_error() const    {  return t == T_OBJECT && u == UT_OBJECT_ERROR; }  // that is TS error

      
      // T_OBJECT only, set value of object's data slot
      void set_object_data(void* pv)
      {
        assert(u == UT_OBJECT_NATIVE);
        ValueBinaryDataSet(this,(LPCBYTE)pv,1,T_OBJECT,0);
      }
      
      // T_OBJECT/UT_OBJECT_FUNCTION only, call TS function
      // 'self' here is what will be known as 'this' inside the function, can be undefined for invocations of global functions 
      value call( int argc, const value* argv, value self = value(), const WCHAR* url_or_script_name = 0) const
      {
        value rv;
        ValueInvoke(const_cast<value*>(this),&self,argc,argv,&rv,LPCWSTR(url_or_script_name));
        return rv;
      }

      value call() const {  return call(0,0);  }
      value call( const value& p1 ) const {  return call(1,&p1); }
      value call( const value& p1, const value& p2 )  const { value args[2] = { p1,p2 };  return call(2,args); }
      value call( const value& p1, const value& p2, const value& p3 ) const { value args[3] = { p1,p2,p3 };  return call(3,args); }
      value call( const value& p1, const value& p2, const value& p3, const value& p4 )  const { value args[4] = { p1,p2,p3,p4 };  return call(4,args); }

      /** converts T_OBJECT/UT_OBJECT_*** values into plain map of key/value pairs */
      void isolate()
      {
        ValueIsolate(this);
      }

      // "smart" or "soft" equality test
      static bool equal(const value& v1, const value& v2)
      {
        if( v1 == v2 ) return true; // strict comparison
        switch ( v1.t > v2.t? v1.t: v2.t )
        {
           case T_BOOL:
             {
               bool const r1 = v1.get(false);
               bool const r2 = v2.get(!r1);
               return r1 == r2;
             }
           case T_INT:
             {
               int const r1 = v1.get(0);
               int const r2 = v2.get(-r1);
               return r1 == r2;
             }
           case T_FLOAT:
             {
               double const r1 = v1.get(0.0);
               double const r2 = v2.get(-r1);
               return r1 == r2;
             }
        }
        return false;
      }

    };

    inline int getter(const value& v, int*) { return v.get(0); }
    inline unsigned getter(const value& v, unsigned*) { return (unsigned)v.get(0); }
    inline bool     getter(const value& v, bool*) { return v.get(false); }
    inline double   getter(const value& v, double*) { return v.get(0.0); }
    inline float    getter(const value& v, float*) { return (float)v.get(0.0); }
    inline string   getter(const value& v, string*) { return v.to_string(); }
#ifdef CPP11
    inline astring  getter(const value& v, astring*) { aux::w2utf a(v.to_string()); return astring(a.c_str(), a.length()); }
#endif
    inline value    getter(const value& v, value*) { return v; }

    inline std::vector<byte>
      getter(const value& v, std::vector<byte>*) { aux::bytes bs = v.get_bytes(); return std::vector<byte>(bs.start, bs.end()); }

    template<typename T> inline std::vector<T>
      getter(const value& v, std::vector<T>*) {
        std::vector<T> out;
        if (v.is_array_like()) {
          int n = v.length();
          for (int i = 0; i < n; ++i) out.push_back(v.get_item(i).get<T>());
        }
        return out;
      }

    // value by key bidirectional proxy/accessor 
    class value_key_a
    {
      friend class value;
      value& col;
      value  key;
      value_key_a& operator=(const value_key_a& val); // no such thing
    protected:
      value_key_a( value& c, const value& k ): col(c),key(k) {}
    public:
      ~value_key_a() {}
      value_key_a& operator= (const value& val) { col.set_item(key,val); return *this; }
    };

    inline value_key_a 
        value::operator[](const value& key) { return value_key_a(*this, key); }

    // value by index bidirectional proxy/accessor 
    class value_idx_a
    {
      friend class value;
      value& col;
      int    idx;
      value_idx_a& operator= (const value_idx_a& val); // no such thing
    protected:
      value_idx_a( value& c, int i ): col(c), idx(i) {}
    public:
      ~value_idx_a() {}
      value_idx_a& operator= (const value& val) { col.set_item(idx,val); return *this; }
    };

    inline value_idx_a 
        value::operator[](int idx) { return value_idx_a(*this, idx); }

    inline value::value(const value_key_a& src) {
      ValueInit(this);
      *this = src.col.get_item(src.key);
    }

    inline value::value(const value_idx_a& src) {
      ValueInit(this);
      *this = src.col.get_item(src.idx);
    }

  }


#ifdef CPP11
  namespace sciter {

    class native_function
    {
    public:
      native_function(const native_function_t& f): func(f) { assert(f); }
      virtual ~native_function() {}

      virtual bool invoke( unsigned int argc, const VALUE* argv, VALUE* retval ) 
      {
        if( func ) { 
          ValueInit(retval);
          value r = func(argc,static_cast<const value*>(argv));
          ValueCopy(retval,&r);
          return true;
        }
        else
          return false;
      }
      native_function_t func;
    private:
      native_function(const native_function& f);
      native_function& operator=(const native_function& f);
    public:
      static VOID invoke_impl( VOID* tag, UINT argc, const VALUE* argv, VALUE* retval) 
      {
        native_function* self = static_cast<native_function*>(tag);
        ValueInit(retval);
        value r = self->func(argc,static_cast<const value*>(argv));
        ValueCopy(retval,&r);
      }
      static VOID release_impl( VOID* tag ) 
      {
        native_function* self = static_cast<native_function*>(tag);
        delete self;
      }
    };

    inline value::value( const native_function_t& nfr ) {
      ValueInit(this); 
      native_function* pnf = new native_function(nfr);
      ValueNativeFunctorSet(this, native_function::invoke_impl, native_function::release_impl, pnf );
    }

    // value(native function) is a wrapper that produces sciter::value from native function
    // see uminimal sample 

    template<typename R>
    inline value setter(R(*func)()) {
      native_function_t tf = [func](unsigned int argc, const value* argv) -> value { R r = func(); return value(r); };
      return value(tf);
    }

    template<typename R, typename T1> 
    inline value setter( R(*func)(T1 t1) ) {
      native_function_t tf = [func](unsigned int argc, const value* argv) -> value {
        R r = func(argc >= 1 ? argv[0].get<T1>() : T1());
        return value(r);
      };
      return value(tf); 
    }

    template<typename R, typename T1, typename T2> 
    inline value setter( R(*func)(T1 t1,T2 t2) ) {
      native_function_t tf = [func](unsigned int argc, const value* argv) -> value {
        R r = func(argc >= 1 ? argv[0].get<T1>() : T1(),
          argc >= 2 ? argv[1].get<T2>() : T2());
        return value(r);
      };
      return value(tf); 
    }
    template<typename R, typename T1, typename T2, typename T3> 
    inline value setter( R(*func)(T1 t1,T2 t2,T3 t3) ) {
      native_function_t tf = [func](unsigned int argc, const value* argv) -> value {
        R r = func(argc >= 1 ? argv[0].get<T1>() : T1(),
          argc >= 2 ? argv[1].get<T2>() : T2(),
          argc >= 3 ? argv[2].get<T3>() : T3());
        return value(r);
      };
      return value(tf); 
    }
    template<typename R, typename T1, typename T2, typename T3, typename T4> 
    inline value setter( R(*func)(T1 t1,T2 t2,T3 t3,T4 t4) ) {
      native_function_t tf = [func](unsigned int argc, const value* argv) -> value { 
          R r = func(argc >= 1? argv[0].get<T1>(): T1(),
                    argc >= 2? argv[1].get<T2>(): T2(),
                    argc >= 3? argv[2].get<T3>(): T3(),
                    argc >= 4? argv[3].get<T4>(): T4()); 
          return value(r); 
      }; 
      return value(tf);
    }

    template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
    inline value setter( R(*func)(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)) {
      native_function_t tf = [func](unsigned int argc, const value *argv) -> value {
        R r = func(argc >= 1 ? argv[0].get<T1>() : T1(),
          argc >= 2 ? argv[1].get<T2>() : T2(),
          argc >= 3 ? argv[2].get<T3>() : T3(),
          argc >= 4 ? argv[3].get<T4>() : T4(),
          argc >= 5 ? argv[4].get<T5>() : T5());
        return value(r);
      };
      return value(tf);
    }
    template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline value setter( R(*func)(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)) {
      native_function_t tf = [func](unsigned int argc, const value *argv) -> value {
        R r = func(argc >= 1 ? argv[0].get<T1>() : T1(),
          argc >= 2 ? argv[1].get<T2>() : T2(),
          argc >= 3 ? argv[2].get<T3>() : T3(),
          argc >= 4 ? argv[3].get<T4>() : T4(),
          argc >= 5 ? argv[4].get<T5>() : T5(),
          argc >= 6 ? argv[5].get<T6>() : T6());
        return value(r);
      };
      return value(tf);
    }

    // versions of the above but for generic std::function
    template<typename R>
      inline value setter( std::function<R()> func ) 
      {
        native_function_t tf = [func](unsigned int argc, const value* argv) -> value { R r = func(); return value(r); };
        return value(tf);
      }

    template<typename R,typename P0>
      inline value setter( std::function<R(P0)> func )
      {
        native_function_t tf = [func](unsigned int argc, const value* argv) -> value {
          R r = func(argc >= 1 ? argv[0].get<P0>() : P0());
          return value(r);
        };
        return value(tf);
      }
    template<typename R,typename P0,typename P1>
      inline value setter( std::function<R(P0,P1)> func )
      {
        native_function_t tf = [func](unsigned int argc, const value* argv) -> value { 
          R r = func(argc >= 1? argv[0].get<P0>(): P0(),
                     argc >= 2? argv[1].get<P1>(): P1() ); 
          return value(r); 
        };
        return value(tf);
      }

    template<typename R, typename P0, typename P1, typename P2> 
      inline value setter( std::function<R(P0,P1,P2)> func )
      {
        native_function_t tf = [func](unsigned int argc, const value* argv) -> value { 
          R r = func(argc >= 1? argv[0].get<P0>(): P0(),
                    argc >= 2? argv[1].get<P1>(): P1(),
                    argc >= 3? argv[2].get<P2>(): P2()); 
          return value(r); 
        }; 
        return value(tf);
      }

    template<typename R, typename P0, typename P1, typename P2, typename P3> 
      inline value setter( std::function<R(P0,P1,P2,P3)> func ) {
        native_function_t tf = [func](unsigned int argc, const value* argv) -> value { 
            R r = func(argc >= 1? argv[0].get<P0>(): P0(),
                      argc >= 2? argv[1].get<P1>(): P1(),
                      argc >= 3? argv[2].get<P2>(): P2(),
                      argc >= 4? argv[3].get<P3>(): P3()); 
            return value(r); 
        }; 
        return value(tf);
      }

  }
#endif

  #pragma warning( pop )

#endif
