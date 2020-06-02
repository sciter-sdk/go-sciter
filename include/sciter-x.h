/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

/*
 * Sciter main include file
 */


#ifndef __SCITER_X__
#define __SCITER_X__

#if __cplusplus > 199711L
#define CPP11
#endif

#include "sciter-x-types.h"
#include "sciter-x-def.h"
#include "sciter-x-dom.h"
#include "sciter-x-value.h"
#include "sciter-x-api.h"
#include "sciter-x-msg.h"
#include "sciter-om.h"

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

// C++, namespace sciter things
#include "sciter-x-dom.hpp"
#include "sciter-x-host-callback.h"
#include "sciter-x-debug.h"
//#include "sciter-x-threads.h"
#endif

#endif

