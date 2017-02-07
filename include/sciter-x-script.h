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
 * Main include file of Sciter script primitives.  
 */


#ifndef __sciter_x_scripting_h__
#define __sciter_x_scripting_h__

#include "sciter-x.h"
#include "sciter-x-value.h"
#include "tiscript.h"

typedef tiscript_VM* HVM;

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )
  #include "tiscript.hpp"
  namespace sciter
  {
    inline SCITER_VALUE v2v(tiscript::VM* vm, tiscript::value val, bool isolate = true)
    {
      SCITER_VALUE v;
      BOOL r = Sciter_v2V(vm,val,(VALUE*)&v, BOOL(isolate));
      assert(r); r;
      return v;
    }
    inline tiscript::value v2v(tiscript::VM* vm, const SCITER_VALUE& val)
    {
      tiscript::value v;
      BOOL r = Sciter_V2v(vm,(const VALUE*)&val,&v);
      assert(r); r;
      return v;
    }
  }
#endif


#endif
