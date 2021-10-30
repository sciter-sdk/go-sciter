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
 * sciter::value, aka variant, aka discriminated union
 */


#ifndef __sciter_x_value_h__
#define __sciter_x_value_h__

#pragma once

#define HAS_TISCRIPT

#include "value.h"

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )
  typedef sciter::value SCITER_VALUE;
  #include "value.hpp"
#else
  typedef VALUE SCITER_VALUE;
#endif
   
#endif
