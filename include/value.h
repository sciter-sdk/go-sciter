#ifndef __value_h__
#define __value_h__

#include "sciter-x-primitives.h"

typedef enum VALUE_RESULT
{
  HV_OK_TRUE = -1,
  HV_OK = 0,
  HV_BAD_PARAMETER = 1,
  HV_INCOMPATIBLE_TYPE = 2
} VALUE_RESULT;

typedef struct
{
  UINT     t;
  UINT     u;
  UINT64   d;
} VALUE;

#define FLOAT_VALUE   double

typedef enum VALUE_TYPE
{
    T_UNDEFINED = 0,
    T_NULL = 1,
    T_BOOL = 2,
    T_INT  = 3,
    T_FLOAT = 4,
    T_STRING = 5,
    T_DATE = 6,     // INT64 - contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC), a.k.a. FILETIME on Windows
    T_CURRENCY = 7, // INT64 - 14.4 fixed number. E.g. dollars = int64 / 10000; 
    T_LENGTH = 8,   // length units, value is int or float, units are VALUE_UNIT_TYPE
    T_ARRAY = 9,
    T_MAP = 10,
    T_FUNCTION = 11,   // named tuple , like array but with name tag
    T_BYTES = 12,      // sequence of bytes - e.g. image data
    T_OBJECT = 13,     // scripting object proxy (TISCRIPT/SCITER)
    //T_DOM_OBJECT = 14,  // DOM object, use get_object_data to get HELEMENT 
    T_RESOURCE = 15,  // 15 - other thing derived from tool::resource
    //T_RANGE = 16,     // 16 - N..M, integer range.
    T_DURATION = 17,   // double, seconds
    T_ANGLE = 18,      // double, radians
    T_COLOR = 19,      // [unsigned] INT, ABGR
    T_ASSET = 21,      // sciter::om::iasset* add_ref'ed pointer
} VALUE_TYPE;

typedef enum VALUE_UNIT_TYPE
{
    UT_EM = 1, //height of the element's font. 
    UT_EX = 2, //height of letter 'x' 
    UT_PR = 3, //%
    UT_SP = 4, //%% "springs", a.k.a. flex units
    reserved1 = 5, 
    reserved2 = 6, 
    UT_PX = 7, //pixels
    UT_IN = 8, //inches (1 inch = 2.54 centimeters). 
    UT_CM = 9, //centimeters. 
    UT_MM = 10, //millimeters. 
    UT_PT = 11, //points (1 point = 1/72 inches). 
    UT_PC = 12, //picas (1 pica = 12 points). 
    UT_DIP = 13,
    reserved3 = 14, 
    reserved4 = 15, 
    UT_URL   = 16,  // url in string
} VALUE_UNIT_TYPE;

typedef enum VALUE_UNIT_TYPE_DATE
{
    DT_HAS_DATE         = 0x01, // date contains date portion
    DT_HAS_TIME         = 0x02, // date contains time portion HH:MM
    DT_HAS_SECONDS      = 0x04, // date contains time and seconds HH:MM:SS
    DT_UTC              = 0x10, // T_DATE is known to be UTC. Otherwise it is local date/time
} VALUE_UNIT_TYPE_DATE;

// Sciter or TIScript specific
typedef enum VALUE_UNIT_TYPE_OBJECT
{
    UT_OBJECT_ARRAY  = 0,   // type T_OBJECT of type Array
    UT_OBJECT_OBJECT = 1,   // type T_OBJECT of type Object
    UT_OBJECT_CLASS  = 2,   // type T_OBJECT of type Class (class or namespace)
    UT_OBJECT_NATIVE = 3,   // type T_OBJECT of native Type with data slot (LPVOID)
    UT_OBJECT_FUNCTION = 4, // type T_OBJECT of type Function
    UT_OBJECT_ERROR = 5,    // type T_OBJECT of type Error
} VALUE_UNIT_TYPE_OBJECT;

typedef enum VALUE_UNIT_UNDEFINED {
  UT_NOTHING = 1 // T_UNDEFINED && UT_NOTHING -  'nothing' a.k.a. 'void' value in script 
} VALUE_UNIT_UNDEFINED;

// Sciter or TIScript specific
typedef enum VALUE_UNIT_TYPE_STRING
{
    UT_STRING_STRING = 0,        // string
    UT_STRING_ERROR  = 1,        // is an error string
    UT_STRING_SECURE = 2,        // secure string ("wiped" on destroy)
    UT_STRING_SYMBOL = 0xffff,   // symbol in tiscript sense
} VALUE_UNIT_TYPE_STRING;

// Native functor
typedef VOID NATIVE_FUNCTOR_INVOKE( VOID* tag, UINT argc, const VALUE* argv, VALUE* retval); // retval may contain error definition
typedef VOID NATIVE_FUNCTOR_RELEASE( VOID* tag );


/**
 * ValueInit - initialize VALUE storage
 * This call has to be made before passing VALUE* to any other functions
 */
UINT SCAPI ValueInit( VALUE* pval );

/**
 * ValueClear - clears the VALUE and deallocates all assosiated structures that are not used anywhere else.
 */
UINT SCAPI ValueClear( VALUE* pval );

/**
 * ValueCompare - compares two values, returns HV_OK_TRUE if val1 == val2.
 */
UINT SCAPI ValueCompare( const VALUE* pval1, const VALUE* pval2 );

/**
 * ValueCopy - copies src VALUE to dst VALUE. dst VALUE must be in ValueInit state.
 */
UINT SCAPI ValueCopy( VALUE* pdst, const VALUE* psrc );

/**
 * ValueIsolate - converts T_OBJECT value types to T_MAP or T_ARRAY.
 * use this method if you need to pass values between different threads.
 * The fanction is applicable for the Sciter 
 */
UINT SCAPI ValueIsolate( VALUE* pdst );

/**
 * ValueType - returns VALUE_TYPE and VALUE_UNIT_TYPE flags of the VALUE
 */
UINT SCAPI ValueType( const VALUE* pval, UINT* pType, UINT* pUnits );

/**
 * ValueStringData - returns string data for T_STRING type
 * For T_FUNCTION returns name of the fuction. 
 */
UINT SCAPI ValueStringData( const VALUE* pval, LPCWSTR* pChars, UINT* pNumChars );

/**
 * ValueStringDataSet - sets VALUE to T_STRING type and copies chars/numChars to
 * internal refcounted buffer assosiated with the value. 
 */
UINT SCAPI ValueStringDataSet( VALUE* pval, LPCWSTR chars, UINT numChars, UINT units );

/**
 * ValueIntData - retreive integer data of T_INT, T_LENGTH and T_BOOL types
 */
UINT SCAPI ValueIntData( const VALUE* pval, INT* pData );

/**
 * ValueIntDataSet - sets VALUE integer data of T_INT and T_BOOL types 
 * Optionally sets units field too.
 */
UINT SCAPI ValueIntDataSet( VALUE* pval, INT data, UINT type, UINT units );

/**
 * ValueInt64Data - retreive 64bit integer data of T_CURRENCY and T_DATE values.
 */
UINT SCAPI ValueInt64Data( const VALUE* pval, INT64* pData );

/**
 * ValueInt64DataSet - sets 64bit integer data of T_CURRENCY and T_DATE values.
 */
UINT SCAPI ValueInt64DataSet( VALUE* pval, INT64 data, UINT type, UINT units );

/**
 * ValueFloatData - retreive FLOAT_VALUE (double) data of T_FLOAT and T_LENGTH values.
 */
UINT SCAPI ValueFloatData( const VALUE* pval, FLOAT_VALUE* pData );

/**
 * ValueFloatDataSet - sets FLOAT_VALUE data of T_FLOAT and T_LENGTH values.
 */
UINT SCAPI ValueFloatDataSet( VALUE* pval, FLOAT_VALUE data, UINT type, UINT units );

/**
 * ValueBinaryData - retreive integer data of T_BYTES type
 */
UINT SCAPI ValueBinaryData( const VALUE* pval, LPCBYTE* pBytes, UINT* pnBytes );

/**
 * ValueBinaryDataSet - sets VALUE to sequence of bytes of type T_BYTES 
 * 'type' here must be set to T_BYTES. Optionally sets units field too.
 * The function creates local copy of bytes in its own storage.
 */
UINT SCAPI ValueBinaryDataSet( VALUE* pval, LPCBYTE pBytes, UINT nBytes, UINT type, UINT units );

/**
 * ValueElementsCount - retreive number of sub-elements for:
 * - T_ARRAY - number of elements in the array; 
 * - T_MAP - number of key/value pairs in the map;
 * - T_FUNCTION - number of arguments in the function;
 */
UINT SCAPI ValueElementsCount( const VALUE* pval, INT* pn);

/**
 * ValueNthElementValue - retreive value of sub-element at index n for:
 * - T_ARRAY - nth element of the array; 
 * - T_MAP - value of nth key/value pair in the map;
 * - T_FUNCTION - value of nth argument of the function;
 */
UINT SCAPI ValueNthElementValue( const VALUE* pval, INT n, VALUE* pretval);

/**
 * ValueNthElementValueSet - sets value of sub-element at index n for:
 * - T_ARRAY - nth element of the array; 
 * - T_MAP - value of nth key/value pair in the map;
 * - T_FUNCTION - value of nth argument of the function;
 * If the VALUE is not of one of types above then it makes it of type T_ARRAY with 
 * single element - 'val_to_set'.
 */
UINT SCAPI ValueNthElementValueSet( VALUE* pval, INT n, const VALUE* pval_to_set);

/**Callback function used with #ValueEnumElements().
 * return TRUE to continue enumeration
 */
typedef SBOOL SC_CALLBACK KeyValueCallback( LPVOID param, const VALUE* pkey, const VALUE* pval );

/**
 * ValueEnumElements - enumeartes key/value pairs of T_MAP, T_FUNCTION and T_OBJECT values
 * - T_MAP - key of nth key/value pair in the map;
 * - T_FUNCTION - name of nth argument of the function (if any);
 */
UINT SCAPI ValueNthElementKey( const VALUE* pval, INT n, VALUE* pretval);

UINT SCAPI ValueEnumElements( const VALUE* pval, KeyValueCallback* penum, LPVOID param);

/**
 * ValueSetValueToKey - sets value of sub-element by key:
 * - T_MAP - value of key/value pair with the key;
 * - T_FUNCTION - value of argument with the name key;
 * - T_OBJECT (tiscript) - value of property of the object
 * If the VALUE is not of one of types above then it makes it of type T_MAP with 
 * single pair - 'key'/'val_to_set'.
 *
 * key usually is a value of type T_STRING
 *
 */
UINT SCAPI ValueSetValueToKey( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set);

/**
 * ValueGetValueOfKey - retrieves value of sub-element by key:
 * - T_MAP - value of key/value pair with the key;
 * - T_FUNCTION - value of argument with the name key;
 * - T_OBJECT (tiscript) - value of property of the object
 * Otherwise *pretval will have T_UNDEFINED value.
 */
UINT SCAPI ValueGetValueOfKey( const VALUE* pval, const VALUE* pkey, VALUE* pretval);

typedef enum VALUE_STRING_CVT_TYPE
{
  CVT_SIMPLE,        ///< simple conversion of terminal values 
  CVT_JSON_LITERAL,  ///< json literal parsing/emission 
  CVT_JSON_MAP,      ///< json parsing/emission, it parses as if token '{' already recognized 
  CVT_XJSON_LITERAL, ///< x-json parsing/emission, date is emitted as ISO8601 date literal, currency is emitted in the form DDDD$CCC
} VALUE_STRING_CVT_TYPE;

/**
 * ValueToString - converts value to T_STRING inplace:
 * - CVT_SIMPLE - parse/emit terminal values (T_INT, T_FLOAT, T_LENGTH, T_STRING)
 * - CVT_JSON_LITERAL - parse/emit value using JSON literal rules: {}, [], "string", true, false, null 
 * - CVT_JSON_MAP - parse/emit MAP value without enclosing '{' and '}' brackets.
 */
UINT SCAPI ValueToString( VALUE* pval, /*VALUE_STRING_CVT_TYPE*/ UINT how );

/**
 * ValueFromString - parses string into value:
 * - CVT_SIMPLE - parse/emit terminal values (T_INT, T_FLOAT, T_LENGTH, T_STRING), "guess" non-strict parsing
 * - CVT_JSON_LITERAL - parse/emit value using JSON literal rules: {}, [], "string", true, false, null 
 * - CVT_JSON_MAP - parse/emit MAP value without enclosing '{' and '}' brackets.
 * Returns:
 *   Number of non-parsed characters in case of errors. Thus if string was parsed in full it returns 0 (success)  
 */
UINT SCAPI ValueFromString( VALUE* pval, LPCWSTR str, UINT strLength, /*VALUE_STRING_CVT_TYPE*/ UINT how );

/**
 * ValueInvoke - function invocation (Sciter/TIScript).
 * - VALUE* pval is a value of type T_OBJECT/UT_OBJECT_FUNCTION
 * - VALUE* pthis - object that will be known as 'this' inside that function.
 * - UINT argc, const VALUE* argv - vector of arguments to pass to the function. 
 * - VALUE* pretval - parse/emit MAP value without enclosing '{' and '}' brackets.
 * - LPCWSTR url - url or name of the script - used for error reporting in the script.
 * Returns:
 *   HV_OK, HV_BAD_PARAMETER or HV_INCOMPATIBLE_TYPE
 */
UINT SCAPI ValueInvoke( const VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url);

/**
 * ValueNativeFunctorSet - set reference to native function
 * - VALUE* pval - value to be initialized 
 * - NATIVE_FUNCTOR_INVOKE* pinvoke - reference to native functor implementation.
 * - NATIVE_FUNCTOR_RELEASE* prelease - reference to native functor dtor implementation.
 * - VOID* tag - optional tag, passed as it is to pinvoke and prelease
 * Returns:
 *   HV_OK, HV_BAD_PARAMETER
 */
UINT SCAPI ValueNativeFunctorSet( VALUE* pval, 
    NATIVE_FUNCTOR_INVOKE*  pinvoke,
    NATIVE_FUNCTOR_RELEASE* prelease /* = NULL*/,
    VOID* tag /* = NULL*/ );

SBOOL SCAPI ValueIsNativeFunctor( const VALUE* pval);


#if defined(__cplusplus) && !defined(__value_hpp__) && !defined(PLAIN_API_ONLY)

  #include "value.hpp"

  #pragma warning( push )
  #pragma warning(disable:4786) //identifier was truncated...

  namespace json
  {
    using sciter::value;
    using sciter::string;
    using sciter::astring;
  }

#endif //defined(__cplusplus)

#endif
