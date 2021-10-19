/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2015-2020, Terra Informatica Software, Inc.
 */

/*
 * sciter::lite - high level windowless (Sciter.Lite)  wrapper
 * Use these if you need instance of wndowless Sciter.
 * Check /demos.lite/sciter-directx sample.
 */

#pragma once

#ifndef __sciter_x_windowless_hpp__
#define __sciter_x_windowless_hpp__

#include "sciter-x-types.h"
#include "sciter-x.h"
#include "sciter-x-dom.h"
#include "sciter-x-api.h"
#include "sciter-x-dom.hpp"
#include "sciter-x-host-callback.h"
#include "aux-asset.h"
#include "aux-slice.h"
#include <algorithm>

/**sciter namespace.*/
namespace sciter
{

  class lite 
    : public sciter::event_handler
    , public sciter::host<lite>
  {
  public:
    lite(UINT backend = GFX_LAYER_SKIA_OPENGL) { // or GFX_LAYER_D2D
      SBOOL r = SciterProcX(this, SCITER_X_MSG_CREATE(backend, FALSE));
      assert(r);
      if (r) {
        setup_callback();
        sciter::attach_dom_event_handler(this, this);
      }
    }
    virtual ~lite() {
      SciterProcX(this, SCITER_X_MSG_DESTROY());
    }

    virtual long asset_add_ref() { return 0; /* life cycle is governed by owner */ }
    virtual long asset_release() { return 0; /* life cycle is governed by owner */ }

    bool load(aux::bytes utf8_html, const WCHAR* base_url = 0)
    {
      return FALSE != ::SciterLoadHtml(this, utf8_html.start, UINT(utf8_html.length), base_url);
    }
    bool load(aux::chars utf8_html, const WCHAR* base_url = 0)
    {
      return FALSE != ::SciterLoadHtml(this, (LPCBYTE)utf8_html.start, UINT(utf8_html.length), base_url);
    }
    bool load(const WCHAR* url)
    {
      return FALSE != ::SciterLoadFile(this, url);
    }

  // sciter::host traits:
    HWINDOW   get_hwnd() const { return (LPVOID)this; }
    HINSTANCE get_resource_instance() const { return NULL; }

   

  protected:

   };
}

#endif
