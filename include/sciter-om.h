#pragma once

#ifndef __SCITER_OM_H__
#define __SCITER_OM_H__

#include "sciter-x-primitives.h"
#include "sciter-x-value.h"

struct som_passport_t;

typedef UINT64 som_atom_t;

typedef struct som_asset_t {
  struct som_asset_class_t* isa;
} som_asset_t;

typedef struct som_asset_class_t {
  long(*asset_add_ref)(som_asset_t* thing);
  long(*asset_release)(som_asset_t* thing);
  long(*asset_get_interface)(som_asset_t* thing, const char* name, void** out);
  struct som_passport_t* (*asset_get_passport)(som_asset_t* thing);
} som_asset_class_t;

inline struct som_asset_class_t* som_asset_get_class(const struct som_asset_t* pass)
{
#ifdef __cplusplus
  return pass ? pass->isa : nullptr;
#else
  return pass ? pass->isa : NULL;
#endif
}

som_atom_t SCAPI SciterAtomValue(const char* name);

#ifdef CPP11

#include <cstring>
#include <cassert>
#include <atomic>

namespace sciter {

  class atom {
    som_atom_t _atom;
  public:
    atom(const char* name) { _atom = SciterAtomValue(name); }
    atom(const atom& other) { _atom = other._atom; }
    operator som_atom_t() const { return _atom; }
  };

  namespace om {

    template <class R> class hasset;

    // implementation of som_asset_t ISA
    // note: does not define asset_add_ref()/asset_release() as they shall be defined in specializations 
    template <class A>
    class iasset : public som_asset_t
    {
    public:
      iasset() { isa = get_asset_class(); }

      // mandatory:
      virtual long  asset_add_ref() = 0;
      virtual long  asset_release() = 0;
      // can be used to as to get interface as to check if the thing supports the feature
      virtual long  asset_get_interface(const char* name, void** out = nullptr) {
        if (0 != strcmp(name, interface_name())) return false;
        if (out) { this->asset_add_ref(); *out = this; }
        return true;
      }
      virtual som_passport_t* asset_get_passport() const { 
        return nullptr; 
      }

      static som_asset_class_t* get_asset_class() {
        static som_asset_class_t cls = {
          &thunk_asset_add_ref,
          &thunk_asset_release,
          &thunk_asset_get_interface,
          &thunk_asset_get_passport
        };
        return &cls;
      };

      static long thunk_asset_add_ref(som_asset_t* thing) { return static_cast<A*>(thing)->asset_add_ref(); }
      static long thunk_asset_release(som_asset_t* thing) { return static_cast<A*>(thing)->asset_release(); }
      static long thunk_asset_get_interface(som_asset_t* thing, const char* name, void** out) { return static_cast<A*>(thing)->asset_get_interface(name, out); }
      static som_passport_t* thunk_asset_get_passport(som_asset_t* thing) { return static_cast<A*>(thing)->asset_get_passport(); }

      static const char* interface_name() { return "asset.sciter.com"; }
      //template<class C> hasset<C> interface_of() { hasset<C> p; get_interface(C::interface_name(), p.target()); return p; }
    };
    
    inline long asset_add_ref(som_asset_t *ptr) {
      assert(ptr);
      assert(ptr->isa);
      assert(ptr->isa->asset_add_ref);
      return ptr->isa->asset_add_ref(ptr);
    }
    inline long asset_release(som_asset_t *ptr) {
      assert(ptr);
      assert(ptr->isa);
      assert(ptr->isa->asset_release);
      return ptr->isa->asset_release(ptr);
    }
    inline long asset_get_interface(som_asset_t *ptr, const char* name, void** out = nullptr) {
      assert(ptr);
      assert(ptr->isa);
      assert(ptr->isa->asset_get_interface);
      return ptr->isa->asset_get_interface(ptr, name, out);
    }
    
    inline som_passport_t* asset_get_passport(som_asset_t *ptr) {
      assert(ptr);
      assert(ptr->isa);
      assert(ptr->isa->asset_get_passport);
      return ptr->isa->asset_get_passport(ptr);
    }

    inline som_asset_class_t* asset_get_class(som_asset_t *ptr) {
      assert(ptr);
      return ptr->isa;
    }
    
    //hasset - yet another shared_ptr
    //         R here is an entity derived from som_asset_t
    template <class R> class hasset
    {
    protected:
      R* p;

    public:
      typedef R asset_t;

      hasset() :p(nullptr) {}
      hasset(R* lp) :p(nullptr) { if (lp) asset_add_ref(p = lp); }
      hasset(const hasset<R>& cp) :p(nullptr) { if (cp.p) asset_add_ref(p = cp.p); }

      ~hasset() { if (p)	asset_release(p); }
      operator R*() const { return p; }
      R* operator->() const { assert(p != 0); return p; }

      bool operator!() const { return p == 0; }
      explicit operator bool() const { return  p != 0; }
      bool operator!=(R* pR) const { return p != pR; }
      bool operator==(R* pR) const { return p == pR; }

      // release the interface and set it to NULL
      void release() { if (p) { R* pt = p; p = 0; asset_release(pt); } }

      // attach to an existing interface (does not AddRef)
      void attach(R* p2) { asset_release(p); p = p2; }
      // detach the interface (does not Release)
      R* detach() { R* pt = p; p = 0;	return pt; }

      static R* assign(R* &pp, R* lp)
      {
        if (lp != 0) asset_add_ref(lp);
        if (pp) asset_release(pp);
        pp = lp;
        return lp;
      }

      R* operator=(R* lp) { if (p != lp) return assign(p, lp); return p; }
      R* operator=(const hasset<R>& lp) { if (p != lp) return assign(p, lp.p); return p; }

      void** target() { release(); return (void**)&p; }

    };
    
    // reference counted asset, uses intrusive add_ref/release counter
    template<class C> 
      class asset : public iasset<asset<C>>
    {
      std::atomic<long> _ref_cntr;
    public:
      asset() :_ref_cntr(0) {}
      asset(const asset&/*r*/) = delete;

      virtual ~asset()
      {
        assert(_ref_cntr == 0);
      }

      long  asset_release() override
      {
        assert(_ref_cntr > 0);
        long t = --_ref_cntr;
        if (t == 0)
          asset_finalize();
        return t;
      }
      long  asset_add_ref() override { return ++_ref_cntr; }

      // "name" here is an arbitrary name that includes domain name in reversed order:
      //    "element.dom.sciter.com"
      //    "video-renderer.dom.sciter.com"
      //virtual bool  asset_get_interface(const char* name, iasset** out) override { return false; }

      virtual void asset_finalize()
      {
        delete static_cast<C*>(this);
      }
    };
  }

  template <class AT>
  inline AT* value::get_asset() const {
    som_asset_t* pass = get_asset();
    if (pass && (som_asset_get_class(pass) == AT::get_asset_class()))
      return static_cast<AT*>(pass);
    return nullptr;
  }

}

#endif

#include "sciter-om-def.h"

#endif
