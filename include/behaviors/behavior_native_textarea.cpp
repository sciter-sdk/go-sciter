#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"

#ifdef WINDOWS
#ifndef THIS_HINSTANCE
  EXTERN_C IMAGE_DOS_HEADER __ImageBase;
  #define THIS_HINSTANCE ((HINSTANCE)&__ImageBase)
#endif
#endif

using namespace std;

namespace sciter
{
/*
BEHAVIOR: native-clock
  - draws content layer using sciter-x-graphics.hpp primitives.

SAMPLE:
   See: samples/behaviors/native-drawing.htm
*/

#ifdef WINDOWS

struct native_textarea: public event_handler
{
  HWND     this_hwnd;
  HELEMENT this_element;
    // ctor
  native_textarea(): this_hwnd(0), this_element(0) {}
    virtual ~native_textarea() {}

    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_BEHAVIOR_EVENT | HANDLE_METHOD_CALL;
      return true;
    }

    virtual void attached  (HELEMENT he ) 
    {
      this_element = he;

      dom::element self = dom::element::element(this_element);

      RECT rc = self.get_location(VIEW_RELATIVE | CONTENT_BOX);

      HWND parent = self.get_element_hwnd(false);

      this_hwnd = ::CreateWindow(TEXT("EDIT"), 
                                 TEXT(""), 
                                 WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN | ES_MULTILINE,
                                 rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                                 parent, NULL, THIS_HINSTANCE, 0);

      ::SendMessage(this_hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

      self.attach_hwnd(this_hwnd);
    }
    virtual void detached  (HELEMENT he ) 
    { 
      if (this_hwnd)
        ::DestroyWindow(this_hwnd);
      asset_release();
    }

    virtual bool handle_method_call(HELEMENT he, METHOD_PARAMS& params) { 
      switch (params.methodID) 
      {
        case GET_VALUE:
        {
          VALUE_PARAMS* pvp = (VALUE_PARAMS*)(&params);
          unsigned int length = ::GetWindowTextLength(this_hwnd);
          wchar_t * buffer = new wchar_t[length];
          ::GetWindowText(this_hwnd,buffer,length);
          pvp->val = sciter::value(buffer, length);
          delete[] buffer;
          return true;
        }
        case SET_VALUE: {
          VALUE_PARAMS* pvp = (VALUE_PARAMS*)(&params);
          std::wstring text = pvp->val.to_string();
          ::SetWindowText(this_hwnd, text.c_str());
          return true;
        }
      }
      return false; 
    }

/*    BEGIN_FUNCTION_MAP
      FUNCTION_6("nativeGetPath", nativeGetPath); 
      FUNCTION_2("nativeImage", nativeImage); 
    END_FUNCTION_MAP */

};
#endif
struct native_textarea_factory: public behavior_factory {

  native_textarea_factory(): behavior_factory("native-textarea") { }

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
#ifdef WINDOWS
    return new native_textarea();
#else 
    return nullptr;
#endif
  }

};

// instantiating and attaching it to the global list
native_textarea_factory native_textarea_factory_instance;


}
