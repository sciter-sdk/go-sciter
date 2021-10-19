#pragma once

#if defined(_WIN32)
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #define _WINSOCKAPI_
  #include <windows.h>
#endif

#include <atomic>
#include "sciter-om.h"

namespace aux {

  using namespace sciter::om;

}

#if defined(WINDOWS)

namespace com {

  //asset - yet another smart pointer
  template <class T>
  class ptr
  {
  protected:
	  T* p;

  public:
	  typedef T asset_t;

	  ptr():p(0)                    {}
	  ptr(T* lp):p(0)               { if (lp) (p = lp)->AddRef();	}
    ptr(const ptr<T>& cp):p(0)    { if (cp.p) (p = cp.p)->AddRef();	}

	  ~ptr()
    {
        ULONG c = 0;
        if (p)
          c = p->Release();
    }
	  operator T*() const	          {	return p;	}
    T* operator->() const         { assert(p != 0); return p; }

    // used as target T** pointer to pointer - in places receiving newly created objects (initially add-refed)
    T** target()                  { release(); return &p; }

    bool operator!() const        {	return p == 0;	}
    operator bool() const         { return p != 0;	}
	  bool operator!=(T* pT) const  {	return p != pT;	}
	  bool operator==(T* pT) const  {	return p == pT;	}

	  // release the interface and set it to NULL
	  void release()                {	if (p)	{ T* pt = p; p = 0; pt->Release(); }}

    // attach to an existing interface (does not AddRef)
	  void attach(T* p2)            { release();	p = p2;	}
	  // detach the interface (does not Release)
	  T* detach()                   {	T* pt = p; p = 0;	return pt; }

    static T* assign(T* &pp, T* lp)
    {
	    if (lp != 0) lp->AddRef();
	    if (pp) pp->Release();
	    pp = lp;
	    return lp;
    }

    T* operator=(T* lp)               { if(p != lp) return assign(p, lp); return p;	}
    T* operator=(const ptr<T>& lp)	{ if(p != lp) return assign(p, lp.p); return p;	}

    T* acquire() { if( p ) p->AddRef(); return p; }

    HRESULT CoCreateInstance (REFCLSID classUUID, DWORD dwClsContext = CLSCTX_INPROC_SERVER)
    {
        HRESULT hr = ::CoCreateInstance (classUUID, 0, dwClsContext, __uuidof (T), (void**) target());
        assert (hr != CO_E_NOTINITIALIZED); // You haven't called CoInitialize for the current thread!
        return hr;
    }

    template <class OTHER_COM_CLASS>
      HRESULT QueryInterface (REFCLSID classUUID, ptr<OTHER_COM_CLASS>& dest_object) const
      {
          if (p == 0)
              return E_POINTER;
          return p->QueryInterface (classUUID, (void**) dest_object.target());
      }

    template <class OTHER_COM_CLASS>
      HRESULT QueryInterface (ptr<OTHER_COM_CLASS>& dest_object) const
      {
          return this->QueryInterface (__uuidof (OTHER_COM_CLASS), dest_object);
      }

  };

}

#endif
