#pragma once

#ifndef __SCITER_OM_DEF_H__
#define __SCITER_OM_DEF_H__

#include "sciter-om.h"

#ifdef __cplusplus
#include <exception>
#endif

#ifdef SCITER_BUILD
  #define SOM_VALUE tool::value
#else
  #define SOM_VALUE SCITER_VALUE
#endif

UINT64 SCAPI SciterAtomValue(const char* name);
UINT64 SCAPI SciterAtomNameCB(UINT64 atomv, LPCSTR_RECEIVER* rcv, LPVOID rcv_param);

typedef SBOOL(*som_prop_getter_t)(som_asset_t* thing, SOM_VALUE* p_value);
typedef SBOOL(*som_prop_setter_t)(som_asset_t* thing, const SOM_VALUE* p_value);
typedef SBOOL(*som_item_getter_t)(som_asset_t* thing, const SOM_VALUE* p_key, SOM_VALUE* p_value);
typedef SBOOL(*som_item_setter_t)(som_asset_t* thing, const SOM_VALUE* p_key, const SOM_VALUE* p_value);
typedef SBOOL(*som_item_next_t)(som_asset_t* thing, SOM_VALUE* p_idx /*in/out*/, SOM_VALUE* p_value);
typedef SBOOL(*som_any_prop_getter_t)(som_asset_t* thing, UINT64 propSymbol, SOM_VALUE* p_value);
typedef SBOOL(*som_any_prop_setter_t)(som_asset_t* thing, UINT64 propSymbol, const SOM_VALUE* p_value);
typedef SBOOL(*som_method_t)(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result);
typedef void(*som_dispose_t)(som_asset_t* thing);

typedef struct som_property_def_t {
  void*             reserved;
  som_atom_t        name;
  som_prop_getter_t getter;
  som_prop_setter_t setter;
#ifdef __cplusplus
  som_property_def_t(const char* n, som_prop_getter_t pg, som_prop_setter_t ps = nullptr) : name(SciterAtomValue(n)), getter(pg), setter(ps) {}
#endif
} som_property_def_t;

typedef struct som_method_def_t {
  void*        reserved;
  som_atom_t   name;
  size_t       params;
  som_method_t func;
#ifdef __cplusplus
  som_method_def_t(const char* n, size_t p, som_method_t f) : name(SciterAtomValue(n)), params(p), func(f) {}
#endif
} som_method_def_t;

enum som_passport_flags {
  SOM_SEALED_OBJECT = 0x00,    // not extendable
  SOM_EXTENDABLE_OBJECT = 0x01 // extendable, asset may have new properties added
};

// definiton of object (the thing) access interface
// this structure should be statically allocated - at least survive last instance of the engine
typedef struct som_passport_t {
  UINT64             flags;
  som_atom_t         name;         // class name
  const som_property_def_t* properties; size_t n_properties; // virtual property thunks
  const som_method_def_t*   methods; size_t n_methods;       // method thunks
  som_item_getter_t  item_getter;  // var item_val = thing[k];
  som_item_setter_t  item_setter;  // thing[k] = item_val;
  som_item_next_t    item_next;    // for(var item in thisThing)
  // any property "inteceptors"
  som_any_prop_getter_t  prop_getter;  // var prop_val = thing.k;
  som_any_prop_setter_t  prop_setter;  // thing.k = prop_val;
} som_passport_t;

#ifdef CPP11

#define SOM_FUNC(name) som_method_def_t( #name,\
    sciter::om::member_function<decltype(&TC::name)>::n_params,\
    &sciter::om::member_function<decltype(&TC::name)>::thunk<&TC::name> )

#define SOM_FUNC_EX(ename,fname) som_method_def_t(#ename,\
    sciter::om::member_function<decltype(&TC::fname)>::n_params, \
    &sciter::om::member_function<decltype(&TC::fname)>::thunk<&TC::fname> )

#define SOM_PROP(name) som_property_def_t(#name,\
    &sciter::om::member_property<TC,decltype(TC::name),&TC::name>::getter,\
    &sciter::om::member_property<TC,decltype(TC::name),&TC::name>::setter)

#define SOM_PROP_EX(ename,pname) som_property_def_t(#ename,\
    &sciter::om::member_property<TC,decltype(TC::pname),&TC::pname>::getter,\
    &sciter::om::member_property<TC,decltype(TC::pname),&TC::pname>::setter)

#define SOM_RO_PROP(name) som_property_def_t(#name, \
    &sciter::om::member_property<TC,decltype(TC::name),&TC::name>::getter)

#define SOM_RO_PROP_EX(ename,pname) som_property_def_t(#ename, \
    &sciter::om::member_property<TC,decltype(TC::pname),&TC::pname>::getter)

#define SOM_VIRTUAL_PROP(name,prop_getter,prop_setter) som_property_def_t(#name, \
    &sciter::om::member_getter_function<decltype(&TC::prop_getter)>::thunk<&TC::prop_getter>,\
    &sciter::om::member_setter_function<decltype(&TC::prop_setter)>::thunk<&TC::prop_setter>)

#define SOM_RO_VIRTUAL_PROP(name,prop_getter) som_property_def_t(#name, \
    &sciter::om::member_getter_function<decltype(&TC::prop_getter)>::thunk<&TC::prop_getter>)

#define SOM_ITEM_SET(func) \
    st.item_setter = &sciter::om::item_set_accessor<decltype(&TC::func)>::thunk<&TC::func>;

#define SOM_ITEM_GET(func) \
    st.item_getter = &sciter::om::item_get_accessor<decltype(&TC::func)>::thunk<&TC::func>;

#define SOM_ITEM_NEXT(func) \
    st.item_next = &sciter::om::item_next_accessor<decltype(&TC::func)>::thunk<&TC::func>;

#define SOM_PROP_SET(func) \
    st.prop_setter = &sciter::om::prop_set_accessor<decltype(&TC::func)>::thunk<&TC::func>;

#define SOM_PROP_GET(func) \
    st.prop_getter = &sciter::om::prop_get_accessor<decltype(&TC::func)>::thunk<&TC::func>;


#define SOM_PASSPORT_BEGIN(classname) \
   static const char* interface_name() { return #classname; } \
   som_passport_t* asset_get_passport() const override { \
     typedef classname TC; \
     static som_passport_t st = {}; \
     st.name = SciterAtomValue(#classname); \

#define SOM_PASSPORT_BEGIN_EX(exname, classname) \
   static const char* interface_name() { return #exname; } \
   som_passport_t* asset_get_passport() const override { \
     typedef classname TC; \
     static som_passport_t st = {}; \
     st.name = SciterAtomValue(#exname); \

#define SOM_PASSPORT_END \
     return &st; \
   }

#define SOM_PASSPORT_FLAGS(fs) st.flags = fs;

#define SOM_FUNCS(...) \
     static som_method_def_t methods[] = { __VA_ARGS__ }; \
     st.methods = methods; \
     st.n_methods = sizeof(methods) / sizeof(methods[0]);

#define SOM_PROPS(...) \
     static som_property_def_t properties[] = { __VA_ARGS__ }; \
     st.properties = properties; \
     st.n_properties = sizeof(properties) / sizeof(properties[0]);

namespace sciter {

  namespace om {

    typedef std::runtime_error exception;

    template <typename Type, typename PT, PT Type::*M>
      struct member_property
      {
        static SBOOL getter(som_asset_t* thing, SOM_VALUE* p_value)
          { *p_value = SOM_VALUE((static_cast<Type*>(thing)->*M)); return TRUE; }
        static SBOOL setter(som_asset_t* thing, const SOM_VALUE* p_value)
          { static_cast<Type*>(thing)->*M = p_value->get<PT>(); return TRUE; }
      };

    template <class Type> struct member_function;

    template <class Type, class Ret>
      struct member_function<Ret(Type::*)()> {
        enum { n_params = 0 };
        template <Ret(Type::*Func)()> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
          {
            try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)()); return TRUE; }
            catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
          }
      };

    template <class Type>
      struct member_function<void(Type::*)()> {
        enum { n_params = 0 };
        template <void(Type::*Func)()> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0>
      struct member_function<Ret(Type::*)(P0)> {
        enum { n_params = 1 };
        template <Ret(Type::*Func)(P0)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
          {
            try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>())); return TRUE; }
            catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
          }
      };

    template <class Type, class P0>
      struct member_function<void(Type::*)(P0)> {
        enum { n_params = 1 };
        template <void(Type::*Func)(P0)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1>
      struct member_function<Ret(Type::*)(P0,P1)> {
        enum { n_params = 2 };
        template <Ret(Type::*Func)(P0,P1)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
          {
            try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>())); return TRUE; }
            catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
          }
      };

    template <class Type, class P0, class P1>
      struct member_function<void(Type::*)(P0, P1)> {
        enum { n_params = 2 };
        template <void(Type::*Func)(P0, P1)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2>
      struct member_function<Ret(Type::*)(P0, P1, P2)> {
        enum { n_params = 3 };
        template <Ret(Type::*Func)(P0, P1, P2)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
          {
            try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>())); return TRUE; }
            catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
          }
      };

    template <class Type, class P0, class P1, class P2>
      struct member_function<void(Type::*)(P0, P1, P2)> {
        enum { n_params = 3 };
        template <void(Type::*Func)(P0, P1, P2)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2, class P3>
      struct member_function<Ret(Type::*)(P0, P1, P2, P3)> {
        enum { n_params = 4 };
        template <Ret(Type::*Func)(P0, P1, P2, P3)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2, class P3>
      struct member_function<void(Type::*)(P0, P1, P2, P3)> {
        enum { n_params = 4 };
        template <void(Type::*Func)(P0, P1, P2, P3)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2, class P3, class P4>
      struct member_function<Ret(Type::*)(P0, P1, P2, P3, P4)> {
        enum { n_params = 5 };
        template <Ret(Type::*Func)(P0, P1, P2, P3, P4)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2, class P3, class P4>
      struct member_function<void(Type::*)(P0, P1, P2, P3, P4)> {
        enum { n_params = 5 };
        template <void(Type::*Func)(P0, P1, P2, P3, P4)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2, class P3, class P4, class P5>
      struct member_function<Ret(Type::*)(P0, P1, P2, P3, P4, P5)> {
        enum { n_params = 6 };
        template <Ret(Type::*Func)(P0, P1, P2, P3, P4, P5)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>(), argv[5].get<P5>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2, class P3, class P4, class P5>
      struct member_function<void(Type::*)(P0, P1, P2, P3, P4, P5)> {
        enum { n_params = 6 };
        template <void(Type::*Func)(P0, P1, P2, P3, P4, P5)> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>(), argv[5].get<P5>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };


    // func() const  variants of the above
    template <class Type, class Ret>
      struct member_function<Ret(Type::*)() const> {
        enum { n_params = 0 };
        template <Ret(Type::*Func)() const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)()); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type>
      struct member_function<void(Type::*)() const> {
        enum { n_params = 0 };
        template <void(Type::*Func)() const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0>
      struct member_function<Ret(Type::*)(P0) const> {
        enum { n_params = 1 };
        template <Ret(Type::*Func)(P0) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0>
      struct member_function<void(Type::*)(P0) const> {
        enum { n_params = 1 };
        template <void(Type::*Func)(P0) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1>
      struct member_function<Ret(Type::*)(P0, P1) const> {
        enum { n_params = 2 };
        template <Ret(Type::*Func)(P0, P1) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1>
      struct member_function<void(Type::*)(P0, P1) const> {
        enum { n_params = 2 };
        template <void(Type::*Func)(P0, P1) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2>
      struct member_function<Ret(Type::*)(P0, P1, P2) const> {
        enum { n_params = 3 };
        template <Ret(Type::*Func)(P0, P1, P2) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2>
      struct member_function<void(Type::*)(P0, P1, P2) const> {
        enum { n_params = 3 };
        template <void(Type::*Func)(P0, P1, P2) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2, class P3>
      struct member_function<Ret(Type::*)(P0, P1, P2, P3) const> {
        enum { n_params = 4 };
        template <Ret(Type::*Func)(P0, P1, P2, P3) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2, class P3>
      struct member_function<void(Type::*)(P0, P1, P2, P3) const> {
        enum { n_params = 4 };
        template <void(Type::*Func)(P0, P1, P2, P3) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class Ret, class P0, class P1, class P2, class P3, class P4>
      struct member_function<Ret(Type::*)(P0, P1, P2, P3, P4) const> {
        enum { n_params = 5 };
        template <Ret(Type::*Func)(P0, P1, P2, P3, P4) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { *p_result = SOM_VALUE((static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>())); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type, class P0, class P1, class P2, class P3, class P4>
      struct member_function<void(Type::*)(P0, P1, P2, P3, P4) const> {
        enum { n_params = 5 };
        template <void(Type::*Func)(P0, P1, P2, P3, P4) const> static SBOOL thunk(som_asset_t* thing, UINT argc, const SOM_VALUE* argv, SOM_VALUE* p_result)
        {
          try { (static_cast<Type*>(thing)->*Func)(argv[0].get<P0>(), argv[1].get<P1>(), argv[2].get<P2>(), argv[3].get<P3>(), argv[4].get<P4>()); *p_result = SOM_VALUE(); return TRUE; }
          catch (exception& e) { *p_result = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

    template <class Type> struct item_set_accessor;

    template <class Type, class TK, class TV>
      struct item_set_accessor<bool(Type::*)(TK,TV)> {
        template <bool(Type::*Func)(TK,TV)>
        static SBOOL thunk(som_asset_t* thing, const SOM_VALUE* p_key, const SOM_VALUE* p_value)
          { return (static_cast<Type*>(thing)->*Func)(p_key->get<TK>(), p_value->get<TV>()) ? TRUE : FALSE; }};

    template <class Type> struct item_get_accessor;

    // val operator[key];
    template <class Type, class TK, class TV>
      struct item_get_accessor<bool(Type::*)(TK, TV)> {
        template <bool(Type::*Func)(TK, TV)>
        static SBOOL thunk(som_asset_t* thing, const SOM_VALUE* p_key, SOM_VALUE* p_value)
        {
            typename std::remove_reference<TV>::type val;
            if ((static_cast<Type*>(thing)->*Func)(p_key->get<TK>(), val)) {
              *p_value = SOM_VALUE(val);
              return TRUE;
            }
            return FALSE;
        }
      };

    // val operator[key] const; variant of the above
    template <class Type, class TK, class TV>
      struct item_get_accessor<bool(Type::*)(TK, TV) const> {
        template <bool(Type::*Func)(TK, TV) const>
        static SBOOL thunk(som_asset_t* thing, const SOM_VALUE* p_key, SOM_VALUE* p_value)
        {
          typename std::remove_reference<TV>::type val;
          if ((static_cast<Type*>(thing)->*Func)(p_key->get<TK>(), val)) {
            *p_value = SOM_VALUE(val);
            return TRUE;
          }
          return FALSE;
        }
      };

    template <class Type> struct item_next_accessor;

    template <class Type, class TValRef, class TIndexRef>
      struct item_next_accessor<bool(Type::*)(TIndexRef,TValRef)>
      {
        template <bool(Type::*Func)(TIndexRef, TValRef)>
        static SBOOL thunk(som_asset_t* thing, SOM_VALUE* p_index, SOM_VALUE* p_value)
        {
          typedef typename std::remove_reference<TIndexRef>::type TIndex;
          typedef typename std::remove_reference<TValRef>::type TVal;
          TIndex index = p_index->get<TIndex>();
          TVal val;
          if ((static_cast<Type*>(thing)->*Func)(index, val)) {
            *p_index = SOM_VALUE(index);
            *p_value = SOM_VALUE(val);
            return TRUE;
          } else
            return FALSE;
        }
      };

      template <class Type> struct member_getter_function;
      template <class Type> struct member_setter_function;

      template <class Type, class Ret>
      struct member_getter_function<Ret(Type::*)()> {
        template <Ret(Type::*Func)()> static SBOOL thunk(som_asset_t* thing, SOM_VALUE* p_value)
        {
          try { *p_value = SOM_VALUE((static_cast<Type*>(thing)->*Func)()); return TRUE; }
          catch (exception& e) { *p_value = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

      template <class Type, class Ret>
      struct member_getter_function<Ret(Type::*)() const> {
        template <Ret(Type::*Func)() const> static SBOOL thunk(som_asset_t* thing, SOM_VALUE* p_value)
        {
          try { *p_value = SOM_VALUE((static_cast<Type*>(thing)->*Func)()); return TRUE; }
          catch (exception& e) { *p_value = SOM_VALUE::make_error(e.what()); return TRUE; }
        }
      };

      template <class Type, class P0>
      struct member_setter_function<bool(Type::*)(P0)> {
        template <bool(Type::*Func)(P0)> static SBOOL thunk(som_asset_t* thing, const SOM_VALUE* p_value)
        {
          //try { bool r = (static_cast<Type*>(thing)->*Func)(p_value->get<P0>()); return r; }
          //catch (exception& e) { *p_value = SOM_VALUE::make_error(e.what()); return TRUE; }
          bool r = (static_cast<Type*>(thing)->*Func)(p_value->get<P0>());
          return r;
        }
      };

      inline std::string atom_name(UINT64 atomv) {
        std::string s;
        SciterAtomNameCB(atomv, &_LPCSTR2ASTRING, &s);
        return s;
      }

      template <class Type> struct prop_set_accessor;


      // bool set_any_prop(const std::string& name, TV val);
      template <class Type, class TV>
      struct prop_set_accessor<bool(Type::*)(const std::string&,  TV)> {
        template <bool(Type::*Func)(const std::string&,TV)>
        static SBOOL thunk(som_asset_t* thing, som_atom_t name, const SOM_VALUE* p_value)
        {
          return (static_cast<Type*>(thing)->*Func)(atom_name(name), p_value->get<TV>()) ? TRUE : FALSE;
        }
      };

      template <class Type> struct prop_get_accessor;
            

      // bool get_any_prop(const std::string& name, TV& val);
      template <class Type, class TV>
      struct prop_get_accessor<bool(Type::*)(const std::string&, TV)> {
        template <bool(Type::*Func)(const std::string&,TV)>
        static SBOOL thunk(som_asset_t* thing, som_atom_t name, SOM_VALUE* p_value)
        {
          typename std::remove_reference<TV>::type val;
          if ((static_cast<Type*>(thing)->*Func)(atom_name(name), val)) {
            *p_value = SOM_VALUE(val);
            return TRUE;
          }
          return FALSE;
        }
      };

      // returns pack of asset's properties as a map
      inline SOM_VALUE asset_to_map(som_asset_t *ptr) {
        if (auto psp = asset_get_passport(ptr)) {
          SOM_VALUE map;
          for (size_t n = 0; n < psp->n_properties; ++n) {
            SOM_VALUE val;
            if (psp->properties[n].getter(ptr, &val))
              map.set_item(atom_name(psp->properties[n].name).c_str(), val);
          }
          return map;
        }
        return SOM_VALUE();
      }

  }
}


#endif // __cplusplus

#endif
