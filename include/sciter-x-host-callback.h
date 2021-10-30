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
 * Sciter host application helpers
 */


#ifndef __sciter_x_host_callback_h__
#define __sciter_x_host_callback_h__

#include "sciter-x-api.h"
#include "sciter-x-debug.h"
#include <assert.h>

#pragma warning(disable:4786) //identifier was truncated...
#pragma warning(disable:4100) //unreferenced formal parameter

#pragma once

/**Sciter namespace.*/
namespace sciter
{

  // Sciter's compressed archive produced by sdk/bin/packfolder.exe
  class archive {
    HSARCHIVE har;
    archive(const archive&);
    archive& operator=(const archive&);
  public:
    archive():har(0) {}
    // open archive blob (NOTE: archive does not copy the data - it has to be available while archive is used )
    bool open( LPCBYTE data, UINT data_length ) { close(); har = SAPI()->SciterOpenArchive(data,data_length); return har != 0; }
    bool open( aux::bytes data ) { return open( data.start, UINT(data.length) ); }
    void close() { if(har) SAPI()->SciterCloseArchive(har); har = 0; }
    // get archive item:
    aux::bytes get( LPCWSTR path ) {
      LPCBYTE pb = 0; UINT blen = 0;
      if( aux::chars_of(path).like(WSTR("//*")))
        path += 2;
      SAPI()->SciterGetArchiveItem(har,path,&pb,&blen); return aux::bytes(pb,blen);
    }

    static archive& instance()
    {
      static archive sar;
      return sar;
    }

  };

  // load data from resource section of the PE
  bool load_resource_data(HINSTANCE hinst, LPCWSTR uri, LPCBYTE& pb, UINT& cb );
  bool load_html_resource(HINSTANCE hinst, UINT resId, LPCBYTE& pb, UINT& cb );

  // standard implementation of SCN_ATTACH_BEHAVIOR notification
  inline bool create_behavior( LPSCN_ATTACH_BEHAVIOR lpab )
  {
    event_handler *pb = behavior_factory::create(lpab->behaviorName, lpab->element);
    if(pb)
    {
      lpab->elementTag  = pb;
      lpab->elementProc = event_handler::element_proc;
      return true;
    }
    return false;
  }


/** \struct host
 *  \brief standard implementation of SCITER_CALLBACK_NOTIFICATION handler.
 *  Supposed to be used as a C++ mixin, see: <a href="http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern">CRTP</a>
 **/

  //Base shall have two methods defined:
  //  HWINDOW   get_hwnd()
  //  HINSTANCE get_resource_instance()


  template <typename BASE>
    struct host
  {

      void setup_callback()
      {
        SciterSetCallback(static_cast< BASE* >(this)->get_hwnd(),callback, static_cast< BASE* >( this ) );
      }
      void setup_callback(HWINDOW hwnd)
      {
        SciterSetCallback(hwnd,callback, static_cast< BASE* >( this ) );
      }

      // Sciter runtime callback
      static UINT SC_CALLBACK callback(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID param)
      {
          assert(param);
          BASE* self = static_cast<BASE*>(param);
          return (UINT)self->handle_notification(pnm);
      }

      // notifiaction cracker
      LRESULT handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm)
      {
        // Crack and call appropriate method

        // here are all notifiactions
        switch(pnm->code)
        {
          case SC_LOAD_DATA:          return static_cast<BASE*>(this)->on_load_data((LPSCN_LOAD_DATA) pnm);
          case SC_DATA_LOADED:        return static_cast<BASE*>(this)->on_data_loaded((LPSCN_DATA_LOADED)pnm);
          //case SC_DOCUMENT_COMPLETE: return on_document_complete();
          case SC_ATTACH_BEHAVIOR:    return static_cast<BASE*>(this)->on_attach_behavior((LPSCN_ATTACH_BEHAVIOR)pnm );
          case SC_ENGINE_DESTROYED:   return static_cast<BASE*>(this)->on_engine_destroyed();
          case SC_POSTED_NOTIFICATION: return static_cast<BASE*>(this)->on_posted_notification((LPSCN_POSTED_NOTIFICATION)pnm);
          case SC_GRAPHICS_CRITICAL_FAILURE: static_cast<BASE*>(this)->on_graphics_critical_failure(); return 0;
        }
        return 0;
      }

      // Overridables

      virtual LRESULT on_load_data(LPSCN_LOAD_DATA pnmld)
      {
        LPCBYTE pb = 0; UINT cb = 0;
        aux::wchars wu = aux::chars_of(pnmld->uri);

        if(wu.like(WSTR("res:*")))
        {
          // then by calling possibly overloaded load_resource_data method
          if (static_cast<BASE*>(this)->load_resource_data(wu.start + 4, pb, cb))
            ::SciterDataReady(pnmld->hwnd, pnmld->uri, pb, cb);
          else {
#ifdef _DEBUG
#ifdef CPP11
            auto console = debug_output::instance();
            if (console)
              console->printf("LOAD FAILURE:%S\n", pnmld->uri);
#endif
#endif
            return LOAD_DISCARD;
          }
        } else if(wu.like(WSTR("this://app/*"))) {
          // try to get them from archive first
          aux::bytes adata = archive::instance().get(wu.start+11);
          if (adata.length)
            ::SciterDataReady(pnmld->hwnd, pnmld->uri, adata.start, UINT(adata.length));
          else {
#ifdef _DEBUG
#ifdef CPP11
            auto console = debug_output::instance();
            if (console)
              console->printf("LOAD FAILURE:%S\n", pnmld->uri);
#endif
#endif
            return LOAD_DISCARD;
          }
        }
        return LOAD_OK;
      }

      virtual LRESULT on_data_loaded(LPSCN_DATA_LOADED pnmld)  {
#ifdef _DEBUG
//        auto console = debug_output::instance();
//        if (console) {
//          if(!pnmld->data || !pnmld->dataSize)
//            console->printf("SC_DATA_LOAD FAILURE (%d) :%S\n", pnmld->status, pnmld->uri);
//          else
//            console->printf("SC_DATA_LOADED:%S\n", pnmld->uri);
//        }
#endif
        return 0; 
      }
      virtual LRESULT on_attach_behavior( LPSCN_ATTACH_BEHAVIOR lpab ) { return create_behavior(lpab); }
      virtual LRESULT on_engine_destroyed( ) { return 0; }
      virtual LRESULT on_posted_notification( LPSCN_POSTED_NOTIFICATION lpab ) { return 0; }

      virtual void on_graphics_critical_failure()
      {
#if defined(WINDOWS) && defined(_DEBUG)
        // Direct2D critical error (on layered window) - it rendered nothing. Most probably bad video driver.
        ::MessageBox(NULL,TEXT("Direct2D graphics critical error"), TEXT("Critical Error"), MB_OK | MB_ICONERROR);
#endif
      }

      bool load_resource_data(LPCWSTR uri, LPCBYTE& pb, UINT& cb )
      {
         return sciter::load_resource_data(static_cast< BASE* >(this)-> get_resource_instance(), uri, pb, cb );
      }

      bool load_file(LPCWSTR uri)
      {
        return SciterLoadFile(static_cast< BASE* >(this)->get_hwnd(), uri ) != 0;
      }
      bool load_html(LPCBYTE pb, UINT cb, LPCWSTR uri = 0)
      {
        return SciterLoadHtml(static_cast< BASE* >(this)->get_hwnd(),pb,cb, uri) != 0;
      }

      HELEMENT root()
      {
        HELEMENT re = 0;
        SCDOM_RESULT r = SciterGetRootElement(static_cast< BASE* >(this)->get_hwnd(),&re);
        assert(r == SCDOM_OK); r = r;
        return re;
      }
      HELEMENT get_root() { return root(); }

      // call scripting function defined in the global namespace
      SCITER_VALUE  call_function(LPCSTR name, UINT argc, SCITER_VALUE* argv )
      {
        HWINDOW hwnd = static_cast< BASE* >(this)->get_hwnd();
        SCITER_VALUE rv;
        SBOOL r = SciterCall(hwnd, name, argc, argv, &rv);
#if !defined(SCITER_SUPPRESS_SCRIPT_ERROR_THROW)
        if( (r == FALSE) && rv.is_error_string()) {
          aux::w2a u8 (rv.get(WSTR("")));
          throw sciter::script_error(u8.c_str());
        }
#endif
        assert(r); r = r;
        return rv;
      }

      // flattened wrappers of the above. note SCITER_VALUE is a json::value
      SCITER_VALUE  call_function(LPCSTR name )
      {
        return call_function(name,0,0);
      }
      SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0 )
      {
        return call_function(name,1,&arg0);
      }
      SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1 )
      {
        SCITER_VALUE argv[2]; argv[0] = arg0; argv[1] = arg1;
        return call_function(name,2,argv);
      }
      SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2 )
      {
        SCITER_VALUE argv[3]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2;
        return call_function(name,3,argv);
      }
      SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2, SCITER_VALUE arg3 )
      {
        SCITER_VALUE argv[4]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2; argv[3] = arg3;
        return call_function(name,4,argv);
      }

      // will post or send custom event to all windows in this GUI thread
      // To subscribe on such events use this in JS:
      //   Window.this.on("event-name",function(evt) {
      //     ...
      //   });
      static bool broadcast_event(BEHAVIOR_EVENT_PARAMS evt, bool post = true)
      {
        assert(evt.name);
        evt.cmd = CUSTOM;    // only custom, named events here 
        evt.heTarget = NULL; // heTarget must be null to dispatch it to all windows
        SBOOL handled = false;
        SCDOM_RESULT r = SciterFireEvent(&evt, post, &handled);
        assert(r == SCDOM_OK); (void)r;
        return handled != 0;
      }

  };

#ifdef WINDOWS

  inline bool load_html_resource(HINSTANCE hinst, UINT resId, LPCBYTE& pb, UINT& cb )
  {
    HRSRC hrsrc = ::FindResource(hinst, MAKEINTRESOURCE(resId), MAKEINTRESOURCE(23));
    if (!hrsrc) return FALSE; // resource not found here - proceed with default loader
    // Load specified resource and check if ok
    HGLOBAL hgres = ::LoadResource(hinst, hrsrc);
    if (!hgres) return FALSE;
    // Retrieve resource data and check if ok
    pb = (PBYTE)::LockResource(hgres); if (!pb) return FALSE;
    cb = ::SizeofResource(hinst, hrsrc); if (!cb) return FALSE;
    return TRUE;
  }

  // loads data from resource section of hinst library.
  // accepts "name.ext" and "res:name.ext" uri's
  inline bool load_resource_data(HINSTANCE hinst,  LPCWSTR uri, LPCBYTE& pb, UINT& cb )
  {

    if (!uri || !uri[0])
      return false;
    // Retrieve url specification into a local storage since FindResource() expects
    // to find its parameters on stack rather then on the heap under Win9x/Me

    if(wcsncmp(uri,L"res:",4) == 0)
      uri += 4;

    if(wcsncmp(uri,L"//",2) == 0)
      uri += 2;

    WCHAR achURL[MAX_PATH]; 
#if defined(_MSC_VER)    
    wcsncpy_s(achURL, uri, MAX_PATH);
#else
    wcsncpy(achURL, uri, MAX_PATH);
#endif

    LPWSTR pszName = achURL;

    // Separate extension if any
    LPWSTR pszExt = wcsrchr(pszName, '.'); if (pszExt) *pszExt++ = '\0';

    // Find specified resource and leave if failed. Note that we use extension
    // as the custom resource type specification or assume standard HTML resource
    // if no extension is specified

    HRSRC hrsrc = 0;
    bool  isHtml = false;
    if( pszExt == 0 || _wcsicmp(pszExt,L"HTML") == 0 || _wcsicmp(pszExt,L"HTM") == 0)
    {
      hrsrc = ::FindResourceW(hinst, pszName, MAKEINTRESOURCEW(23) /*RT_HTML*/);
      isHtml = true;
    }
    else
      hrsrc = ::FindResourceW(hinst, pszName, pszExt);

    if (!hrsrc)
      return false; // resource not found here - proceed with the default loader

    // Load specified resource and check if ok

    HGLOBAL hgres = ::LoadResource(hinst, hrsrc);
    if (!hgres) return false;

    // Retrieve resource data and check if ok

    pb = (PBYTE)::LockResource(hgres); if (!pb) return FALSE;
    cb = ::SizeofResource(hinst, hrsrc); if (!cb) return FALSE;

    // Report data ready

    return true;
  }
#else

  inline bool load_resource_data(HINSTANCE hinst,  LPCWSTR uri, LPCBYTE& pb, UINT& cb ) { return false; }

#endif

}

#endif
