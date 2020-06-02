#ifndef __tis_h__
#define __tis_h__

#include "sciter-x-types.h"

#if defined(__GNUC__)
  #ifdef __LP64__
    #define TIS_CDECL
  #else
    #define TIS_CDECL __attribute__((__cdecl__))
  #endif
#else
  #define TIS_CDECL __cdecl
#endif

#if defined(_WINDOWS) || defined(WIN32) || defined(WIN64)
  #define EXTAPI __stdcall
#else
  #define EXTAPI TIS_CDECL
#endif

#define TISAPI TIS_CDECL

#pragma pack(push,8)

struct tiscript_VM; // TIScript virtual machine
typedef struct tiscript_VM tiscript_VM;
// tiscript_value
typedef UINT64 tiscript_value;
typedef unsigned char byte;

typedef tiscript_VM* HVM;

// pinned tiscript_value, val here will survive GC.
typedef struct tiscript_pvalue
{
   tiscript_value val;
   struct tiscript_VM* vm;
   void *d1,*d2;
} tiscript_pvalue;

struct tiscript_stream;
typedef bool TISAPI  tiscript_stream_input(struct tiscript_stream* tag, int* pv);
typedef bool TISAPI  tiscript_stream_output(struct tiscript_stream* tag, int v);
typedef const WCHAR* TISAPI tiscript_stream_name(struct tiscript_stream* tag);
typedef void TISAPI  tiscript_stream_close(struct tiscript_stream* tag);

typedef struct tiscript_stream_vtbl // stream instance
{
  tiscript_stream_input*    input;
  tiscript_stream_output*   output;
  tiscript_stream_name*     get_name;
  tiscript_stream_close*    close;
} tiscript_stream_vtbl;

typedef struct tiscript_stream
{
  tiscript_stream_vtbl* _vtbl;
} tiscript_stream;

// native method implementation
typedef tiscript_value TISAPI tiscript_method(tiscript_VM *c);
typedef tiscript_value TISAPI tiscript_tagged_method(tiscript_VM *c, tiscript_value self, void* tag);

// [] accessors implementation
typedef tiscript_value TISAPI tiscript_get_item(tiscript_VM *c,tiscript_value obj,tiscript_value key);
typedef void  TISAPI tiscript_set_item(tiscript_VM *c,tiscript_value obj,tiscript_value key, tiscript_value tiscript_value);

// getter/setter implementation
typedef tiscript_value TISAPI tiscript_get_prop(tiscript_VM *c,tiscript_value obj);
typedef void  TISAPI tiscript_set_prop(tiscript_VM *c,tiscript_value obj,tiscript_value tiscript_value);

// iterator function used in for(var el in collection)
typedef tiscript_value TISAPI tiscript_iterator(tiscript_VM *c,tiscript_value* index, tiscript_value obj);

// callbacks for enums below
typedef bool  TISAPI tiscript_object_enum(tiscript_VM *c,tiscript_value key, tiscript_value tiscript_value, void* tag); // true - continue enumeartion

// destructor of native objects
typedef void  TISAPI tiscript_finalizer(tiscript_VM *c,tiscript_value obj);

// GC notifier for native objects
typedef void  TISAPI tiscript_on_gc_copy(void* instance_data, tiscript_value new_self);

// callback used for
typedef void TISAPI tiscript_callback(tiscript_VM *c,void* prm);

typedef struct tiscript_method_def
{
  void*             dispatch; // a.k.a. VTBL
  UINT32            id;
  const char*       name;
  tiscript_method*  handler;  // or tiscript_tagged_method if tag is not 0
  void*             tag;
  tiscript_value    payload;  // must be zero
} tiscript_method_def;

typedef struct tiscript_prop_def
{
  void*                dispatch; // a.k.a. VTBL
  UINT32               id;
  const char*          name;
  tiscript_get_prop*   getter;
  tiscript_set_prop*   setter;
  void*                tag;
} tiscript_prop_def;

#define TISCRIPT_CONST_INT    0
#define TISCRIPT_CONST_FLOAT  1
#define TISCRIPT_CONST_STRING 2

typedef struct tiscript_const_def
{
  const char *name;
  union _val
  {
    int          i;
    double       f;
    const WCHAR* str;
  } val;
  unsigned type;
} tiscript_const_def;


typedef struct tiscript_class_def
{
   const char*   name;      // having this name
   tiscript_method_def*   methods;    // with these methods
   tiscript_prop_def*     props;      // with these properties
   tiscript_const_def*    consts;     // with these constants (if any)
   tiscript_get_item*     get_item;   // var v = obj[idx]
   tiscript_set_item*     set_item;   // obj[idx] = v
   tiscript_finalizer*    finalizer;  // destructor of native objects
   tiscript_iterator*     iterator;   // for(var el in collecton) handler
   tiscript_on_gc_copy*   on_gc_copy; // called by GC to notify that 'self' is moved to new location
   tiscript_value         prototype;  // superclass, prototype for the class (or 0)
} tiscript_class_def;

typedef struct tiscript_native_interface
{
  // create new tiscript_VM [and make it current for the thread].
  tiscript_VM*   (TISAPI *create_vm)(unsigned features /*= 0xffffffff*/, unsigned heap_size /*= 1*1024*1024*/, unsigned stack_size /*= 64*1024*/);
  // destroy tiscript_VM
  void  (TISAPI *destroy_vm)(tiscript_VM* pvm);
  // invoke GC
  void  (TISAPI *invoke_gc)(tiscript_VM* pvm);
  // set stdin, stdout and stderr for this tiscript_VM
  void  (TISAPI *set_std_streams)(tiscript_VM* pvm, tiscript_stream* input, tiscript_stream* output, tiscript_stream* error);
  // get tiscript_VM attached to the current thread
  tiscript_VM*   (TISAPI *get_current_vm)();
  // get global namespace (Object)
  tiscript_value (TISAPI *get_global_ns)(tiscript_VM*);
  // get current namespace (Object)
  tiscript_value (TISAPI *get_current_ns)(tiscript_VM*);

  bool (TISAPI *is_int)(tiscript_value v);
  bool (TISAPI *is_float)(tiscript_value v);
  bool (TISAPI *is_symbol)(tiscript_value v);
  bool (TISAPI *is_string)(tiscript_value v);
  bool (TISAPI *is_array)(tiscript_value v);
  bool (TISAPI *is_object)(tiscript_value v);
  bool (TISAPI *is_native_object)(tiscript_value v);
  bool (TISAPI *is_function)(tiscript_value v);
  bool (TISAPI *is_native_function)(tiscript_value v);
  bool (TISAPI *is_instance_of)(tiscript_value v, tiscript_value cls);
  bool (TISAPI *is_undefined)(tiscript_value v);
  bool (TISAPI *is_nothing)(tiscript_value v);
  bool (TISAPI *is_null)(tiscript_value v);
  bool (TISAPI *is_true)(tiscript_value v);
  bool (TISAPI *is_false)(tiscript_value v);
  bool (TISAPI *is_class)(tiscript_VM*,tiscript_value v);
  bool (TISAPI *is_error)(tiscript_value v);
  bool (TISAPI *is_bytes)(tiscript_value v);
  bool (TISAPI *is_datetime)(tiscript_VM*, tiscript_value v);

  bool (TISAPI *get_int_value)(tiscript_value v, int* pi);
  bool (TISAPI *get_float_value)(tiscript_value v, double* pd);
  bool (TISAPI *get_bool_value)(tiscript_value v, bool* pb);
  bool (TISAPI *get_symbol_value)(tiscript_value v, const WCHAR** psz);
  bool (TISAPI *get_string_value)(tiscript_value v, const WCHAR** pdata, unsigned* plength);
  bool (TISAPI *get_bytes)(tiscript_value v, unsigned char** pb, unsigned* pblen);
  bool (TISAPI *get_datetime)(tiscript_VM*, tiscript_value v, unsigned long long* dt);
                                            // dt - 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC)
                                            // a.k.a. FILETIME in Windows

  tiscript_value (TISAPI *nothing_value)(); // special value that designates "does not exist" result.
  tiscript_value (TISAPI *undefined_value)();
  tiscript_value (TISAPI *null_value)();
  tiscript_value (TISAPI *bool_value)(bool v);
  tiscript_value (TISAPI *int_value)(int v);
  tiscript_value (TISAPI *float_value)(double v);
  tiscript_value (TISAPI *string_value)(tiscript_VM*, const WCHAR* text, unsigned int text_length);
  tiscript_value (TISAPI *symbol_value)(const char* zstr);
  tiscript_value (TISAPI *bytes_value)(tiscript_VM*, const byte* data, unsigned int data_length);
  tiscript_value (TISAPI *datetime_value)(tiscript_VM*, unsigned long long dt);

  tiscript_value (TISAPI *to_string)(tiscript_VM*,tiscript_value v);

  // define native class
  tiscript_value (TISAPI *define_class)
      (
          tiscript_VM*          vm,           // in this tiscript_VM
          tiscript_class_def*   cls,          //
          tiscript_value        zns           // in this namespace object (or 0 if global)
      );

   // object
   tiscript_value    (TISAPI *create_object)(tiscript_VM*, tiscript_value of_class); // of_class == 0 - "Object"
   bool     (TISAPI *set_prop)(tiscript_VM*,tiscript_value obj, tiscript_value key, tiscript_value tiscript_value);
   tiscript_value    (TISAPI *get_prop)(tiscript_VM*,tiscript_value obj, tiscript_value key);
   bool     (TISAPI *for_each_prop)(tiscript_VM*, tiscript_value obj, tiscript_object_enum* cb, void* tag);
   void*    (TISAPI *get_instance_data)(tiscript_value obj);
   void     (TISAPI *set_instance_data)(tiscript_value obj, void* data);

   // array
   tiscript_value    (TISAPI *create_array)(tiscript_VM*, unsigned of_size);
   bool     (TISAPI *set_elem)(tiscript_VM*, tiscript_value obj, unsigned idx, tiscript_value tiscript_value);
   tiscript_value    (TISAPI *get_elem)(tiscript_VM*, tiscript_value obj, unsigned idx);
   tiscript_value    (TISAPI *set_array_size)(tiscript_VM*, tiscript_value obj, unsigned of_size);
   unsigned (TISAPI *get_array_size)(tiscript_VM*, tiscript_value obj);

   // eval
   bool     (TISAPI *eval)(tiscript_VM*, tiscript_value ns, tiscript_stream* input, bool template_mode, tiscript_value* pretval);
   bool     (TISAPI *eval_string)(tiscript_VM*, tiscript_value ns, const WCHAR* script, unsigned script_length, tiscript_value* pretval);
   // call function (method)
   bool     (TISAPI *call)(tiscript_VM*, tiscript_value obj, tiscript_value function, const tiscript_value* argv, unsigned argn, tiscript_value* pretval);

   // compiled bytecodes
   bool     (TISAPI *compile)( tiscript_VM* pvm, tiscript_stream* input, tiscript_stream* output_bytecodes, bool template_mode );
   bool     (TISAPI *loadbc)( tiscript_VM* pvm, tiscript_stream* input_bytecodes );

   // throw error
   void     (TISAPI *throw_error)( tiscript_VM*, const WCHAR* error);

   // arguments access
   unsigned (TISAPI *get_arg_count)( tiscript_VM* pvm );
   tiscript_value    (TISAPI *get_arg_n)( tiscript_VM* pvm, unsigned n );

   // path here is global "path" of the object, something like
   // "one"
   // "one.two", etc.
   bool     (TISAPI *get_value_by_path)(tiscript_VM* pvm, tiscript_value* v, const char* path);

   // pins
   void     (TISAPI *pin)(tiscript_VM*, tiscript_pvalue* pp);
   void     (TISAPI *unpin)(tiscript_pvalue* pp);

   // create native_function_value and native_property_value,
   // use this if you want to add native functions/properties in runtime to exisiting classes or namespaces (including global ns)
   tiscript_value (TISAPI *native_function_value)(tiscript_VM* pvm, tiscript_method_def* p_method_def);
   tiscript_value (TISAPI *native_property_value)(tiscript_VM* pvm, tiscript_prop_def* p_prop_def);

   // Schedule execution of the pfunc(prm) in the thread owning this VM.
   // Used when you need to call scripting methods from threads other than main (GUI) thread
   // It is safe to call tiscript functions inside the pfunc.
   // returns 'true' if scheduling of the call was accepted, 'false' when failure (VM has no dispatcher attached).
   bool           (TISAPI *post)(tiscript_VM* pvm, tiscript_callback* pfunc, void* prm);

   // Introduce alien VM to the host VM:
   // Calls method found on "host_method_path" (if there is any) on the pvm_host
   // notifying the host about other VM (alien) creation. Return value of script function "host_method_path" running in pvm_host is passed
   // as a parametr of a call to function at "alien_method_path".
   // One of possible uses of this function:
   // Function at "host_method_path" creates async streams that will serve a role of stdin, stdout and stderr for the alien vm.
   // This way two VMs can communicate with each other.
   //unsigned      (TISAPI *introduce_vm)(tiscript_VM* pvm_host, const char* host_method_path,  tiscript_VM* pvm_alien, const char* alien_method_path);

   bool  (TISAPI *set_remote_std_streams)(tiscript_VM* pvm, tiscript_pvalue* input, tiscript_pvalue* output, tiscript_pvalue* error);

   // support of multi-return values from native fucntions, n here is a number 1..64
   tiscript_value (TISAPI *make_val_list)(tiscript_VM* pvm, int valc, const tiscript_value* va);

   // returns number of props in object, elements in array, or bytes in byte array.
   int   (TISAPI *get_length)(tiscript_VM* pvm, tiscript_value obj );
   // for( var val in coll ) {...}
   bool  (TISAPI *get_next)(tiscript_VM* pvm, tiscript_value* obj, tiscript_value* pos, tiscript_value* val);
   // for( var (key,val) in coll ) {...}
   bool  (TISAPI *get_next_key_value)(tiscript_VM* pvm, tiscript_value* obj, tiscript_value* pos, tiscript_value* key, tiscript_value* val);

   // associate extra data pointer with the VM
   bool  (TISAPI *set_extra_data)(tiscript_VM* pvm, void* data);
   void* (TISAPI *get_extra_data)(tiscript_VM* pvm);

} tiscript_native_interface;

/*#ifdef TISCRIPT_EXT_MODULE
  extern tiscript_native_interface* TIScriptAPI;
#elif defined(SCITER_EXPORTS)
  EXTERN_C __declspec(dllexport) tiscript_native_interface* EXTAPI TIScriptAPI();
#else
  EXTERN_C __declspec(dllimport) tiscript_native_interface* EXTAPI TIScriptAPI();
#endif*/

// signature of TIScriptLibraryInit function - entry point of TIScript Extnension Library
typedef void EXTAPI  TIScriptLibraryInitFunc(tiscript_VM* vm, tiscript_native_interface* piface );

#pragma pack(pop)

#endif
