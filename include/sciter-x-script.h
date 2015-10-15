/** \mainpage Terra Informatica Sciter engine.
 *
 * \section legal_sec In legalese
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * <a href="http://terrainformatica.com/Sciter">Sciter Home</a>
 *
 * (C) 2003-2006, Terra Informatica Software, Inc. and Andrew Fedoniouk
 *
 * \section structure_sec Structure of the documentation
 *
 * See <a href="files.html">Files</a> section.
 **/

/**\file
 * \brief Application defined scripting classes.
 **/

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
