/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */


#ifndef __SCITER_API_X__
#define __SCITER_API_X__

#include "sciter-x-types.h"
#include "sciter-x-def.h"
#include "sciter-x-dom.h"
#include "sciter-x-request.h"
#include "value.h"
#include "tiscript.hpp"

#if !defined(WINDOWS)
  #include <stdlib.h>
  #include <unistd.h>
  #if defined(OSX)
    #include <libproc.h>
  #endif
#endif

#if defined(OSX)
  #include <dlfcn.h>
#endif

struct SciterGraphicsAPI;

typedef struct _ISciterAPI {

  UINT    version; // is zero for now

  LPCWSTR SCFN( SciterClassName )();
  UINT    SCFN( SciterVersion )(BOOL major);
  BOOL    SCFN( SciterDataReady )(HWINDOW hwnd,LPCWSTR uri,LPCBYTE data, UINT dataLength);
  BOOL    SCFN( SciterDataReadyAsync )(HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength, LPVOID requestId);
#ifdef WINDOWS
  LRESULT SCFN( SciterProc )(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  LRESULT SCFN( SciterProcND )(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* pbHandled);
#endif
  BOOL    SCFN( SciterLoadFile )(HWINDOW hWndSciter, LPCWSTR filename);

  BOOL    SCFN( SciterLoadHtml )(HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl);
  VOID    SCFN( SciterSetCallback )(HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam);
  BOOL    SCFN( SciterSetMasterCSS )(LPCBYTE utf8, UINT numBytes);
  BOOL    SCFN( SciterAppendMasterCSS )(LPCBYTE utf8, UINT numBytes);
  BOOL    SCFN( SciterSetCSS )(HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType);
  BOOL    SCFN( SciterSetMediaType )(HWINDOW hWndSciter, LPCWSTR mediaType);
  BOOL    SCFN( SciterSetMediaVars )(HWINDOW hWndSciter, const SCITER_VALUE *mediaVars);
  UINT    SCFN( SciterGetMinWidth )(HWINDOW hWndSciter);
  UINT    SCFN( SciterGetMinHeight )(HWINDOW hWndSciter, UINT width);
  BOOL    SCFN( SciterCall )(HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval);
  BOOL    SCFN( SciterEval )( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval);
  VOID    SCFN( SciterUpdateWindow)(HWINDOW hwnd);
#ifdef WINDOWS
  BOOL    SCFN( SciterTranslateMessage )(MSG* lpMsg);
#endif
  BOOL    SCFN( SciterSetOption )(HWINDOW hWnd, UINT option, UINT_PTR value );
  VOID    SCFN( SciterGetPPI )(HWINDOW hWndSciter, UINT* px, UINT* py);
  BOOL    SCFN( SciterGetViewExpando )( HWINDOW hwnd, VALUE* pval );
#ifdef WINDOWS
  BOOL    SCFN( SciterRenderD2D )(HWINDOW hWndSciter, ID2D1RenderTarget* prt);
  BOOL    SCFN( SciterD2DFactory )(ID2D1Factory ** ppf);
  BOOL    SCFN( SciterDWFactory )(IDWriteFactory ** ppf);
#endif
  BOOL    SCFN( SciterGraphicsCaps )(LPUINT pcaps);
  BOOL    SCFN( SciterSetHomeURL )(HWINDOW hWndSciter, LPCWSTR baseUrl);
#if defined(OSX)
  HWINDOW SCFN( SciterCreateNSView )( LPRECT frame ); // returns NSView*
#endif
#if defined(LINUX)
  HWINDOW SCFN( SciterCreateWidget )( LPRECT frame ); // returns GtkWidget
#endif

  HWINDOW SCFN( SciterCreateWindow )( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent);
  VOID    SCFN( SciterSetupDebugOutput )(
                HWINDOW               hwndOrNull,// HWINDOW or null if this is global output handler
                LPVOID                param,     // param to be passed "as is" to the pfOutput
                DEBUG_OUTPUT_PROC     pfOutput   // output function, output stream alike thing.
                );
//|
//| DOM Element API
//|
  SCDOM_RESULT SCFN( Sciter_UseElement)(HELEMENT he);
  SCDOM_RESULT SCFN( Sciter_UnuseElement)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetRootElement)(HWINDOW hwnd, HELEMENT *phe);
  SCDOM_RESULT SCFN( SciterGetFocusElement)(HWINDOW hwnd, HELEMENT *phe);
  SCDOM_RESULT SCFN( SciterFindElement)(HWINDOW hwnd, POINT pt, HELEMENT* phe);
  SCDOM_RESULT SCFN( SciterGetChildrenCount)(HELEMENT he, UINT* count);
  SCDOM_RESULT SCFN( SciterGetNthChild)(HELEMENT he, UINT n, HELEMENT* phe);
  SCDOM_RESULT SCFN( SciterGetParentElement)(HELEMENT he, HELEMENT* p_parent_he);
  SCDOM_RESULT SCFN( SciterGetElementHtmlCB)(HELEMENT he, BOOL outer, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterGetElementTextCB)(HELEMENT he, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterSetElementText)(HELEMENT he, LPCWSTR utf16, UINT length);
  SCDOM_RESULT SCFN( SciterGetAttributeCount)(HELEMENT he, LPUINT p_count);
  SCDOM_RESULT SCFN( SciterGetNthAttributeNameCB)(HELEMENT he, UINT n, LPCSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterGetNthAttributeValueCB)(HELEMENT he, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterGetAttributeByNameCB)(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterSetAttributeByName)(HELEMENT he, LPCSTR name, LPCWSTR value);
  SCDOM_RESULT SCFN( SciterClearAttributes)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetElementIndex)(HELEMENT he, LPUINT p_index);
  SCDOM_RESULT SCFN( SciterGetElementType)(HELEMENT he, LPCSTR* p_type);
  SCDOM_RESULT SCFN( SciterGetElementTypeCB)(HELEMENT he, LPCSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterGetStyleAttributeCB)(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterSetStyleAttribute)(HELEMENT he, LPCSTR name, LPCWSTR value);
  SCDOM_RESULT SCFN( SciterGetElementLocation)(HELEMENT he, LPRECT p_location, UINT areas /*ELEMENT_AREAS*/);
  SCDOM_RESULT SCFN( SciterScrollToView)(HELEMENT he, UINT SciterScrollFlags);
  SCDOM_RESULT SCFN( SciterUpdateElement)(HELEMENT he, BOOL andForceRender);
  SCDOM_RESULT SCFN( SciterRefreshElementArea)(HELEMENT he, RECT rc);
  SCDOM_RESULT SCFN( SciterSetCapture)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterReleaseCapture)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetElementHwnd)(HELEMENT he, HWINDOW* p_hwnd, BOOL rootWindow);
  SCDOM_RESULT SCFN( SciterCombineURL)(HELEMENT he, LPWSTR szUrlBuffer, UINT UrlBufferSize);
  SCDOM_RESULT SCFN( SciterSelectElements)(HELEMENT  he, LPCSTR    CSS_selectors, SciterElementCallback* callback, LPVOID param);
  SCDOM_RESULT SCFN( SciterSelectElementsW)(HELEMENT  he, LPCWSTR   CSS_selectors, SciterElementCallback* callback, LPVOID param);
  SCDOM_RESULT SCFN( SciterSelectParent)(HELEMENT  he, LPCSTR    selector, UINT      depth, HELEMENT* heFound);
  SCDOM_RESULT SCFN( SciterSelectParentW)(HELEMENT  he, LPCWSTR   selector, UINT      depth, HELEMENT* heFound);
  SCDOM_RESULT SCFN( SciterSetElementHtml)(HELEMENT he, const BYTE* html, UINT htmlLength, UINT where);
  SCDOM_RESULT SCFN( SciterGetElementUID)(HELEMENT he, UINT* puid);
  SCDOM_RESULT SCFN( SciterGetElementByUID)(HWINDOW hwnd, UINT uid, HELEMENT* phe);
  SCDOM_RESULT SCFN( SciterShowPopup)(HELEMENT hePopup, HELEMENT heAnchor, UINT placement);
  SCDOM_RESULT SCFN( SciterShowPopupAt)(HELEMENT hePopup, POINT pos, BOOL animate);
  SCDOM_RESULT SCFN( SciterHidePopup)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetElementState)( HELEMENT he, UINT* pstateBits);
  SCDOM_RESULT SCFN( SciterSetElementState)( HELEMENT he, UINT stateBitsToSet, UINT stateBitsToClear, BOOL updateView);
  SCDOM_RESULT SCFN( SciterCreateElement)( LPCSTR tagname, LPCWSTR textOrNull, /*out*/ HELEMENT *phe );
  SCDOM_RESULT SCFN( SciterCloneElement)( HELEMENT he, /*out*/ HELEMENT *phe );
  SCDOM_RESULT SCFN( SciterInsertElement)( HELEMENT he, HELEMENT hparent, UINT index );
  SCDOM_RESULT SCFN( SciterDetachElement)( HELEMENT he );
  SCDOM_RESULT SCFN( SciterDeleteElement)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterSetTimer)( HELEMENT he, UINT milliseconds, UINT_PTR timer_id );
  SCDOM_RESULT SCFN( SciterDetachEventHandler)( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag );
  SCDOM_RESULT SCFN( SciterAttachEventHandler)( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag );
  SCDOM_RESULT SCFN( SciterWindowAttachEventHandler)( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag, UINT subscription );
  SCDOM_RESULT SCFN( SciterWindowDetachEventHandler)( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag );
  SCDOM_RESULT SCFN( SciterSendEvent)( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT_PTR reason, /*out*/ BOOL* handled);
  SCDOM_RESULT SCFN( SciterPostEvent)( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT_PTR reason);
  SCDOM_RESULT SCFN( SciterCallBehaviorMethod)(HELEMENT he, struct METHOD_PARAMS* params);
  SCDOM_RESULT SCFN( SciterRequestElementData)( HELEMENT he, LPCWSTR url, UINT dataType, HELEMENT initiator );
  SCDOM_RESULT SCFN( SciterHttpRequest)( HELEMENT        he,           // element to deliver data
          LPCWSTR         url,          // url
          UINT            dataType,     // data type, see SciterResourceType.
          UINT            requestType,  // one of REQUEST_TYPE values
          struct REQUEST_PARAM*  requestParams,// parameters
          UINT            nParams       // number of parameters
          );
  SCDOM_RESULT SCFN( SciterGetScrollInfo)( HELEMENT he, LPPOINT scrollPos, LPRECT viewRect, LPSIZE contentSize );
  SCDOM_RESULT SCFN( SciterSetScrollPos)( HELEMENT he, POINT scrollPos, BOOL smooth );
  SCDOM_RESULT SCFN( SciterGetElementIntrinsicWidths)( HELEMENT he, INT* pMinWidth, INT* pMaxWidth );
  SCDOM_RESULT SCFN( SciterGetElementIntrinsicHeight)( HELEMENT he, INT forWidth, INT* pHeight );
  SCDOM_RESULT SCFN( SciterIsElementVisible)( HELEMENT he, BOOL* pVisible);
  SCDOM_RESULT SCFN( SciterIsElementEnabled)( HELEMENT he, BOOL* pEnabled );
  SCDOM_RESULT SCFN( SciterSortElements)( HELEMENT he, UINT firstIndex, UINT lastIndex, ELEMENT_COMPARATOR* cmpFunc, LPVOID cmpFuncParam );
  SCDOM_RESULT SCFN( SciterSwapElements)( HELEMENT he1, HELEMENT he2 );
  SCDOM_RESULT SCFN( SciterTraverseUIEvent)( UINT evt, LPVOID eventCtlStruct, BOOL* bOutProcessed );
  SCDOM_RESULT SCFN( SciterCallScriptingMethod)( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval );
  SCDOM_RESULT SCFN( SciterCallScriptingFunction)( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval );
  SCDOM_RESULT SCFN( SciterEvalElementScript)( HELEMENT he, LPCWSTR script, UINT scriptLength, VALUE* retval );
  SCDOM_RESULT SCFN( SciterAttachHwndToElement)(HELEMENT he, HWINDOW hwnd);
  SCDOM_RESULT SCFN( SciterControlGetType)( HELEMENT he, /*CTL_TYPE*/ UINT *pType );
  SCDOM_RESULT SCFN( SciterGetValue)( HELEMENT he, VALUE* pval );
  SCDOM_RESULT SCFN( SciterSetValue)( HELEMENT he, const VALUE* pval );
  SCDOM_RESULT SCFN( SciterGetExpando)( HELEMENT he, VALUE* pval, BOOL forceCreation );
  SCDOM_RESULT SCFN( SciterGetObject)( HELEMENT he, tiscript_value* pval, BOOL forceCreation );
  SCDOM_RESULT SCFN( SciterGetElementNamespace)(  HELEMENT he, tiscript_value* pval);
  SCDOM_RESULT SCFN( SciterGetHighlightedElement)(HWINDOW hwnd, HELEMENT* phe);
  SCDOM_RESULT SCFN( SciterSetHighlightedElement)(HWINDOW hwnd, HELEMENT he);
//|
//| DOM Node API
//|
  SCDOM_RESULT SCFN( SciterNodeAddRef)(HNODE hn);
  SCDOM_RESULT SCFN( SciterNodeRelease)(HNODE hn);
  SCDOM_RESULT SCFN( SciterNodeCastFromElement)(HELEMENT he, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodeCastToElement)(HNODE hn, HELEMENT* he);
  SCDOM_RESULT SCFN( SciterNodeFirstChild)(HNODE hn, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodeLastChild)(HNODE hn, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodeNextSibling)(HNODE hn, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodePrevSibling)(HNODE hn, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodeParent)(HNODE hnode, HELEMENT* pheParent);
  SCDOM_RESULT SCFN( SciterNodeNthChild)(HNODE hnode, UINT n, HNODE* phn);
  SCDOM_RESULT SCFN( SciterNodeChildrenCount)(HNODE hnode, UINT* pn);
  SCDOM_RESULT SCFN( SciterNodeType)(HNODE hnode, UINT* pNodeType /*NODE_TYPE*/);
  SCDOM_RESULT SCFN( SciterNodeGetText)(HNODE hnode, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param);
  SCDOM_RESULT SCFN( SciterNodeSetText)(HNODE hnode, LPCWSTR text, UINT textLength);
  SCDOM_RESULT SCFN( SciterNodeInsert)(HNODE hnode, UINT where /*NODE_INS_TARGET*/, HNODE what);
  SCDOM_RESULT SCFN( SciterNodeRemove)(HNODE hnode, BOOL finalize);
  SCDOM_RESULT SCFN( SciterCreateTextNode)(LPCWSTR text, UINT textLength, HNODE* phnode);
  SCDOM_RESULT SCFN( SciterCreateCommentNode)(LPCWSTR text, UINT textLength, HNODE* phnode);
//|
//| Value API
//|
  UINT SCFN( ValueInit )( VALUE* pval );
  UINT SCFN( ValueClear )( VALUE* pval );
  UINT SCFN( ValueCompare )( const VALUE* pval1, const VALUE* pval2 );
  UINT SCFN( ValueCopy )( VALUE* pdst, const VALUE* psrc );
  UINT SCFN( ValueIsolate )( VALUE* pdst );
  UINT SCFN( ValueType )( const VALUE* pval, UINT* pType, UINT* pUnits );
  UINT SCFN( ValueStringData )( const VALUE* pval, LPCWSTR* pChars, UINT* pNumChars );
  UINT SCFN( ValueStringDataSet )( VALUE* pval, LPCWSTR chars, UINT numChars, UINT units );
  UINT SCFN( ValueIntData )( const VALUE* pval, INT* pData );
  UINT SCFN( ValueIntDataSet )( VALUE* pval, INT data, UINT type, UINT units );
  UINT SCFN( ValueInt64Data )( const VALUE* pval, INT64* pData );
  UINT SCFN( ValueInt64DataSet )( VALUE* pval, INT64 data, UINT type, UINT units );
  UINT SCFN( ValueFloatData )( const VALUE* pval, FLOAT_VALUE* pData );
  UINT SCFN( ValueFloatDataSet )( VALUE* pval, FLOAT_VALUE data, UINT type, UINT units );
  UINT SCFN( ValueBinaryData )( const VALUE* pval, LPCBYTE* pBytes, UINT* pnBytes );
  UINT SCFN( ValueBinaryDataSet )( VALUE* pval, LPCBYTE pBytes, UINT nBytes, UINT type, UINT units );
  UINT SCFN( ValueElementsCount )( const VALUE* pval, INT* pn);
  UINT SCFN( ValueNthElementValue )( const VALUE* pval, INT n, VALUE* pretval);
  UINT SCFN( ValueNthElementValueSet )( VALUE* pval, INT n, const VALUE* pval_to_set);
  UINT SCFN( ValueNthElementKey )( const VALUE* pval, INT n, VALUE* pretval);
  UINT SCFN( ValueEnumElements )( const VALUE* pval, KeyValueCallback* penum, LPVOID param);
  UINT SCFN( ValueSetValueToKey )( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set);
  UINT SCFN( ValueGetValueOfKey )( const VALUE* pval, const VALUE* pkey, VALUE* pretval);
  UINT SCFN( ValueToString )( VALUE* pval, /*VALUE_STRING_CVT_TYPE*/ UINT how );
  UINT SCFN( ValueFromString )( VALUE* pval, LPCWSTR str, UINT strLength, /*VALUE_STRING_CVT_TYPE*/ UINT how );
  UINT SCFN( ValueInvoke )( const VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url);
  UINT SCFN( ValueNativeFunctorSet )( VALUE* pval, NATIVE_FUNCTOR_INVOKE*  pinvoke, NATIVE_FUNCTOR_RELEASE* prelease, VOID* tag );
  BOOL SCFN( ValueIsNativeFunctor )( const VALUE* pval);

  // tiscript VM API
  tiscript_native_interface*  SCFN(TIScriptAPI)();

  HVM  SCFN( SciterGetVM )( HWINDOW hwnd );

  BOOL SCFN( Sciter_v2V ) (HVM vm, tiscript_value script_value, VALUE* value, BOOL isolate);
  BOOL SCFN( Sciter_V2v ) (HVM vm, const VALUE* valuev, tiscript_value* script_value);

  HSARCHIVE SCFN( SciterOpenArchive ) (LPCBYTE archiveData, UINT archiveDataLength);
  BOOL SCFN( SciterGetArchiveItem ) (HSARCHIVE harc, LPCWSTR path, LPCBYTE* pdata, UINT* pdataLength);
  BOOL SCFN( SciterCloseArchive ) (HSARCHIVE harc);

  SCDOM_RESULT SCFN( SciterFireEvent)( const BEHAVIOR_EVENT_PARAMS* evt, BOOL post, BOOL *handled );

  LPVOID SCFN( SciterGetCallbackParam )(HWINDOW hwnd);
  UINT_PTR SCFN( SciterPostCallback )(HWINDOW hwnd, UINT_PTR wparam, UINT_PTR lparam, UINT timeoutms);

  LPSciterGraphicsAPI SCFN( GetSciterGraphicsAPI )();
  LPSciterRequestAPI SCFN( GetSciterRequestAPI )();

#ifdef WINDOWS 
    BOOL SCFN( SciterCreateOnDirectXWindow ) (HWINDOW hwnd, IUnknown* pSwapChain); // IDXGISwapChain
    BOOL SCFN( SciterRenderOnDirectXWindow ) (HWINDOW hwnd, HELEMENT elementToRenderOrNull, BOOL frontLayer);
    BOOL SCFN( SciterRenderOnDirectXTexture ) (HWINDOW hwnd, HELEMENT elementToRenderOrNull, IUnknown* surface); // IDXGISurface
#endif


} ISciterAPI;

typedef ISciterAPI* (SCAPI *SciterAPI_ptr)();

// getting ISciterAPI reference:

#ifdef STATIC_LIB

    EXTERN_C ISciterAPI* SCAPI SciterAPI();

    inline ISciterAPI* SAPI( ISciterAPI* ext = nullptr ) {
       static ISciterAPI* _api = nullptr;
       if( ext ) _api = ext;
       if( !_api )
       {
          _api = SciterAPI();
          tiscript::ni( _api->TIScriptAPI() );
       }
       assert(_api);
       return _api;
    }

#elif defined(WINDOWS)

    inline ISciterAPI* SAPI( ISciterAPI* ext = NULL ) {
       static ISciterAPI* _api = NULL;
       if( ext ) _api = ext;
       if( !_api )
       {
          HMODULE hm = LoadLibrary( TEXT(SCITER_DLL_NAME) );
          //#if defined(WIN64) || defined(_WIN64)
          //  TEXT("sciter64.dll")
          //#else
          //  TEXT("sciter32.dll")
          //#endif
          //);
          if(hm) {
            SciterAPI_ptr sciterAPI = (SciterAPI_ptr) GetProcAddress(hm, "SciterAPI");
            if( sciterAPI ) {
              _api = sciterAPI();
#if defined(__cplusplus) && !defined(PLAIN_API_ONLY)
              tiscript::ni( _api->TIScriptAPI() );
#endif
            } else {
              FreeLibrary(hm);
            }

          }
       }
       assert(_api);
       if( !_api ) {
         //::MessageBox(NULL, TEXT("Sciter engine not found, quiting"),TEXT("Error"),MB_OK);
         exit(-1);
       }
       return _api;
    }


#elif defined(OSX)

    //typedef ISciterAPI* SCAPI (*SciterAPI_ptr)();

    inline ISciterAPI* SAPI( ISciterAPI* ext = nullptr )
    {
        static ISciterAPI* _api = nullptr;
        if( ext ) _api = ext;
        if( !_api ) {
            pid_t pid;
            char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
            char folderpath[PROC_PIDPATHINFO_MAXSIZE];
            pid = getpid();
            proc_pidpath (pid, pathbuf, sizeof(pathbuf));
            realpath(pathbuf, folderpath);
            *strrchr(folderpath, '/') = '\0';

            void* lib_sciter_handle = dlopen(SCITER_DLL_NAME, RTLD_LOCAL|RTLD_LAZY);
            if( !lib_sciter_handle ) {
                const char* lookup_paths[] =
                {
                    "/" SCITER_DLL_NAME,
                    "/../Frameworks/" SCITER_DLL_NAME, // - (bundle folder)/Contents/Frameworks/
                    "/../../../" SCITER_DLL_NAME // aside of bundle folder (SDK samples)
                };
                for( int n = 0; !lib_sciter_handle && n < 3; ++n ) {
                    char tpath[PROC_PIDPATHINFO_MAXSIZE];
                    strcpy(tpath,folderpath);
                    strcat(tpath, lookup_paths[n]);
                    lib_sciter_handle = dlopen(tpath, RTLD_LOCAL|RTLD_LAZY);
                }
            }
            if (!lib_sciter_handle) {
                fprintf(stderr, "[%s] Unable to load library: %s\n", __FILE__, dlerror());
                exit(EXIT_FAILURE);
            }

            SciterAPI_ptr sapi = (SciterAPI_ptr) dlsym(lib_sciter_handle, "SciterAPI");
            if (!sapi) {
                fprintf(stderr,"[%s] Unable to get symbol: %s\n", __FILE__, dlerror());
                exit(EXIT_FAILURE);
            }
            _api = sapi();
            tiscript::ni( _api->TIScriptAPI() );
        }
        assert(_api);
        return _api;
    }

#elif defined(LINUX)

    #include <libgen.h>
    #include <dlfcn.h>

    //typedef ISciterAPI* SCAPI (*SciterAPI_ptr)();

    inline ISciterAPI* SAPI( ISciterAPI* ext = nullptr )
    {
        static ISciterAPI* _api = nullptr;
        if( ext ) _api = ext;
        if( !_api ) {

            char folderpath[2048] = {0};

            if (readlink ("/proc/self/exe", folderpath, sizeof(folderpath)) != -1)
            {
               dirname (folderpath);
               //strcat  (pathbuf, "/");
            }

            void* lib_sciter_handle = dlopen(SCITER_DLL_NAME, RTLD_LOCAL|RTLD_LAZY);
            if( !lib_sciter_handle ) {
                const char* lookup_paths[] =
                {
                    "/" SCITER_DLL_NAME,
                    nullptr
                };
                for( int n = 0; !lib_sciter_handle; ++n ) {
                    if( !lookup_paths[n] )
                      break;
                    char tpath[2048] = {0};
                    strcpy(tpath,folderpath);
                    strcat(tpath, lookup_paths[n]);
                    lib_sciter_handle = dlopen(tpath, RTLD_LOCAL | RTLD_LAZY);
                }
            }
            if (!lib_sciter_handle) {
                fprintf(stderr, "[%s] Unable to load library: %s\n", __FILE__, dlerror());
                exit(EXIT_FAILURE);
            }

            SciterAPI_ptr sapi = (SciterAPI_ptr) dlsym(lib_sciter_handle, "SciterAPI");
            if (!sapi) {
                fprintf(stderr,"[%s] Unable to get symbol: %s\n", __FILE__, dlerror());
                exit(EXIT_FAILURE);
            }
            _api = sapi();
            tiscript::ni( _api->TIScriptAPI() );
        }
        assert(_api);
        return _api;
    }


#endif

  inline LPSciterGraphicsAPI gapi()
  {
    static LPSciterGraphicsAPI _gapi = SAPI()->GetSciterGraphicsAPI();
    return _gapi;
  }

  inline LPSciterRequestAPI rapi()
  {
    static LPSciterRequestAPI _rapi = SAPI()->GetSciterRequestAPI();
    return _rapi;
  }


  // defining "official" API functions:

  inline   LPCWSTR SCAPI SciterClassName () { return SAPI()->SciterClassName(); }
  inline   UINT    SCAPI SciterVersion (BOOL major) { return SAPI()->SciterVersion (major); }
  inline   BOOL    SCAPI SciterDataReady (HWINDOW hwnd,LPCWSTR uri,LPCBYTE data, UINT dataLength) { return SAPI()->SciterDataReady (hwnd,uri,data,dataLength); }
  inline   BOOL    SCAPI SciterDataReadyAsync (HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength, LPVOID requestId) { return SAPI()->SciterDataReadyAsync (hwnd,uri, data, dataLength, requestId); }
#ifdef WINDOWS
  inline   LRESULT SCAPI SciterProc (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { return SAPI()->SciterProc (hwnd,msg,wParam,lParam); }
  inline   LRESULT SCAPI SciterProcND (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* pbHandled) { return SAPI()->SciterProcND (hwnd,msg,wParam,lParam,pbHandled); }
#endif
  inline   BOOL    SCAPI SciterLoadFile (HWINDOW hWndSciter, LPCWSTR filename) { return SAPI()->SciterLoadFile (hWndSciter,filename); }
  inline   BOOL    SCAPI SciterLoadHtml (HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl) { return SAPI()->SciterLoadHtml (hWndSciter,html,htmlSize,baseUrl); }
  inline   VOID    SCAPI SciterSetCallback (HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam) { SAPI()->SciterSetCallback (hWndSciter,cb,cbParam); }
  inline   BOOL    SCAPI SciterSetMasterCSS (LPCBYTE utf8, UINT numBytes) { return SAPI()->SciterSetMasterCSS (utf8,numBytes); }
  inline   BOOL    SCAPI SciterAppendMasterCSS (LPCBYTE utf8, UINT numBytes) { return SAPI()->SciterAppendMasterCSS (utf8,numBytes); }
  inline   BOOL    SCAPI SciterSetCSS (HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType) { return SAPI()->SciterSetCSS (hWndSciter,utf8,numBytes,baseUrl,mediaType); }
  inline   BOOL    SCAPI SciterSetMediaType (HWINDOW hWndSciter, LPCWSTR mediaType) { return SAPI()->SciterSetMediaType (hWndSciter,mediaType); }
  inline   BOOL    SCAPI SciterSetMediaVars (HWINDOW hWndSciter, const SCITER_VALUE *mediaVars) { return SAPI()->SciterSetMediaVars (hWndSciter,mediaVars); }
  inline   UINT    SCAPI SciterGetMinWidth (HWINDOW hWndSciter) { return SAPI()->SciterGetMinWidth (hWndSciter); }
  inline   UINT    SCAPI SciterGetMinHeight (HWINDOW hWndSciter, UINT width) { return SAPI()->SciterGetMinHeight (hWndSciter,width); }
  inline   BOOL    SCAPI SciterCall (HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval) { return SAPI()->SciterCall (hWnd,functionName, argc,argv,retval); }
  inline   BOOL    SCAPI SciterEval ( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval) { return SAPI()->SciterEval ( hwnd, script, scriptLength, pretval); }
  inline   VOID    SCAPI SciterUpdateWindow(HWINDOW hwnd) { SAPI()->SciterUpdateWindow(hwnd); }
#ifdef WINDOWS
  inline   BOOL    SCAPI SciterTranslateMessage (MSG* lpMsg) { return SAPI()->SciterTranslateMessage (lpMsg); }
#endif
  inline  BOOL    SCAPI SciterSetOption (HWINDOW hWnd, UINT option, UINT_PTR value ) { return SAPI()->SciterSetOption (hWnd,option,value ); }
  inline  VOID    SCAPI SciterGetPPI (HWINDOW hWndSciter, UINT* px, UINT* py) { SAPI()->SciterGetPPI (hWndSciter,px,py); }
  inline  BOOL    SCAPI SciterGetViewExpando ( HWINDOW hwnd, VALUE* pval ) { return SAPI()->SciterGetViewExpando ( hwnd, pval ); }
#ifdef WINDOWS
  inline  BOOL    SCAPI SciterRenderD2D (HWINDOW hWndSciter, ID2D1RenderTarget* prt) { return SAPI()->SciterRenderD2D (hWndSciter,prt); }
  inline  BOOL    SCAPI SciterD2DFactory (ID2D1Factory ** ppf) { return SAPI()->SciterD2DFactory (ppf); }
  inline  BOOL    SCAPI SciterDWFactory (IDWriteFactory ** ppf) { return SAPI()->SciterDWFactory (ppf); }
#endif
  inline  BOOL    SCAPI SciterGraphicsCaps (LPUINT pcaps) { return SAPI()->SciterGraphicsCaps (pcaps); }
  inline  BOOL    SCAPI SciterSetHomeURL (HWINDOW hWndSciter, LPCWSTR baseUrl) { return SAPI()->SciterSetHomeURL (hWndSciter,baseUrl); }
#if defined(OSX)
  inline  HWINDOW SCAPI SciterCreateNSView ( LPRECT frame ) { return SAPI()->SciterCreateNSView ( frame ); }
#endif
  inline  HWINDOW SCAPI SciterCreateWindow ( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent) { return SAPI()->SciterCreateWindow (creationFlags,frame,delegate,delegateParam,parent); }

  inline SCDOM_RESULT SCAPI Sciter_UseElement(HELEMENT he) { return SAPI()->Sciter_UseElement(he); }
  inline SCDOM_RESULT SCAPI Sciter_UnuseElement(HELEMENT he) { return SAPI()->Sciter_UnuseElement(he); }
  inline SCDOM_RESULT SCAPI SciterGetRootElement(HWINDOW hwnd, HELEMENT *phe) { return SAPI()->SciterGetRootElement(hwnd, phe); }
  inline SCDOM_RESULT SCAPI SciterGetFocusElement(HWINDOW hwnd, HELEMENT *phe) { return SAPI()->SciterGetFocusElement(hwnd, phe); }
  inline SCDOM_RESULT SCAPI SciterFindElement(HWINDOW hwnd, POINT pt, HELEMENT* phe) { return SAPI()->SciterFindElement(hwnd,pt,phe); }
  inline SCDOM_RESULT SCAPI SciterGetChildrenCount(HELEMENT he, UINT* count) { return SAPI()->SciterGetChildrenCount(he, count); }
  inline SCDOM_RESULT SCAPI SciterGetNthChild(HELEMENT he, UINT n, HELEMENT* phe) { return SAPI()->SciterGetNthChild(he,n,phe); }
  inline SCDOM_RESULT SCAPI SciterGetParentElement(HELEMENT he, HELEMENT* p_parent_he) { return SAPI()->SciterGetParentElement(he,p_parent_he); }
  inline SCDOM_RESULT SCAPI SciterGetElementHtmlCB(HELEMENT he, BOOL outer, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetElementHtmlCB( he, outer, rcv, rcv_param); }
  inline SCDOM_RESULT SCAPI SciterGetElementTextCB(HELEMENT he, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetElementTextCB(he, rcv, rcv_param); }
  inline SCDOM_RESULT SCAPI SciterSetElementText(HELEMENT he, LPCWSTR utf16, UINT length) { return SAPI()->SciterSetElementText(he, utf16, length); }
  inline SCDOM_RESULT SCAPI SciterGetAttributeCount(HELEMENT he, LPUINT p_count) { return SAPI()->SciterGetAttributeCount(he, p_count); }
  inline SCDOM_RESULT SCAPI SciterGetNthAttributeNameCB(HELEMENT he, UINT n, LPCSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetNthAttributeNameCB(he,n,rcv,rcv_param); }
  inline SCDOM_RESULT SCAPI SciterGetNthAttributeValueCB(HELEMENT he, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetNthAttributeValueCB(he, n, rcv, rcv_param); }
  inline SCDOM_RESULT SCAPI SciterGetAttributeByNameCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetAttributeByNameCB(he,name,rcv,rcv_param); }
  inline SCDOM_RESULT SCAPI SciterSetAttributeByName(HELEMENT he, LPCSTR name, LPCWSTR value) { return SAPI()->SciterSetAttributeByName(he,name,value); }
  inline SCDOM_RESULT SCAPI SciterClearAttributes(HELEMENT he) { return SAPI()->SciterClearAttributes(he); }
  inline SCDOM_RESULT SCAPI SciterGetElementIndex(HELEMENT he, LPUINT p_index) { return SAPI()->SciterGetElementIndex(he,p_index); }
  inline SCDOM_RESULT SCAPI SciterGetElementType(HELEMENT he, LPCSTR* p_type) { return SAPI()->SciterGetElementType(he,p_type); }
  inline SCDOM_RESULT SCAPI SciterGetElementTypeCB(HELEMENT he, LPCSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetElementTypeCB(he,rcv,rcv_param); }
  inline SCDOM_RESULT SCAPI SciterGetStyleAttributeCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterGetStyleAttributeCB(he,name,rcv,rcv_param); }
  inline SCDOM_RESULT SCAPI SciterSetStyleAttribute(HELEMENT he, LPCSTR name, LPCWSTR value) { return SAPI()->SciterSetStyleAttribute(he,name,value); }
  inline SCDOM_RESULT SCAPI SciterGetElementLocation(HELEMENT he, LPRECT p_location, UINT areas /*ELEMENT_AREAS*/) { return SAPI()->SciterGetElementLocation(he,p_location,areas); }
  inline SCDOM_RESULT SCAPI SciterScrollToView(HELEMENT he, UINT SciterScrollFlags) { return SAPI()->SciterScrollToView(he,SciterScrollFlags); }
  inline SCDOM_RESULT SCAPI SciterUpdateElement(HELEMENT he, BOOL andForceRender) { return SAPI()->SciterUpdateElement(he,andForceRender); }
  inline SCDOM_RESULT SCAPI SciterRefreshElementArea(HELEMENT he, RECT rc) { return SAPI()->SciterRefreshElementArea(he,rc); }
  inline SCDOM_RESULT SCAPI SciterSetCapture(HELEMENT he) { return SAPI()->SciterSetCapture(he); }
  inline SCDOM_RESULT SCAPI SciterReleaseCapture(HELEMENT he) { return SAPI()->SciterReleaseCapture(he); }
  inline SCDOM_RESULT SCAPI SciterGetElementHwnd(HELEMENT he, HWINDOW* p_hwnd, BOOL rootWindow) { return SAPI()->SciterGetElementHwnd(he,p_hwnd,rootWindow); }
  inline SCDOM_RESULT SCAPI SciterCombineURL(HELEMENT he, LPWSTR szUrlBuffer, UINT UrlBufferSize) { return SAPI()->SciterCombineURL(he,szUrlBuffer,UrlBufferSize); }
  inline SCDOM_RESULT SCAPI SciterSelectElements(HELEMENT  he, LPCSTR    CSS_selectors, SciterElementCallback* callback, LPVOID param) { return SAPI()->SciterSelectElements(he,CSS_selectors,callback,param); }
  inline SCDOM_RESULT SCAPI SciterSelectElementsW(HELEMENT  he, LPCWSTR   CSS_selectors, SciterElementCallback* callback, LPVOID param) { return SAPI()->SciterSelectElementsW(he,CSS_selectors,callback,param); }
  inline SCDOM_RESULT SCAPI SciterSelectParent(HELEMENT  he, LPCSTR    selector, UINT      depth, HELEMENT* heFound) { return SAPI()->SciterSelectParent(he,selector,depth,heFound); }
  inline SCDOM_RESULT SCAPI SciterSelectParentW(HELEMENT  he, LPCWSTR   selector, UINT      depth, HELEMENT* heFound) { return SAPI()->SciterSelectParentW(he,selector,depth,heFound); }
  inline SCDOM_RESULT SCAPI SciterSetElementHtml(HELEMENT he, const BYTE* html, UINT htmlLength, UINT where) { return SAPI()->SciterSetElementHtml(he,html,htmlLength,where); }
  inline SCDOM_RESULT SCAPI SciterGetElementUID(HELEMENT he, UINT* puid) { return SAPI()->SciterGetElementUID(he,puid); }
  inline SCDOM_RESULT SCAPI SciterGetElementByUID(HWINDOW hwnd, UINT uid, HELEMENT* phe) { return SAPI()->SciterGetElementByUID(hwnd,uid,phe); }
  inline SCDOM_RESULT SCAPI SciterShowPopup(HELEMENT hePopup, HELEMENT heAnchor, UINT placement) { return SAPI()->SciterShowPopup(hePopup,heAnchor,placement); }
  inline SCDOM_RESULT SCAPI SciterShowPopupAt(HELEMENT hePopup, POINT pos, BOOL animate) { return SAPI()->SciterShowPopupAt(hePopup,pos,animate); }
  inline SCDOM_RESULT SCAPI SciterHidePopup(HELEMENT he) { return SAPI()->SciterHidePopup(he); }
  inline SCDOM_RESULT SCAPI SciterGetElementState( HELEMENT he, UINT* pstateBits) { return SAPI()->SciterGetElementState(he,pstateBits); }
  inline SCDOM_RESULT SCAPI SciterSetElementState( HELEMENT he, UINT stateBitsToSet, UINT stateBitsToClear, BOOL updateView) { return SAPI()->SciterSetElementState(he,stateBitsToSet,stateBitsToClear,updateView); }
  inline SCDOM_RESULT SCAPI SciterCreateElement( LPCSTR tagname, LPCWSTR textOrNull, /*out*/ HELEMENT *phe ) { return SAPI()->SciterCreateElement(tagname,textOrNull,phe ); }
  inline SCDOM_RESULT SCAPI SciterCloneElement( HELEMENT he, /*out*/ HELEMENT *phe ) { return SAPI()->SciterCloneElement(he,phe ); }
  inline SCDOM_RESULT SCAPI SciterInsertElement( HELEMENT he, HELEMENT hparent, UINT index ) { return SAPI()->SciterInsertElement(he,hparent,index ); }
  inline SCDOM_RESULT SCAPI SciterDetachElement( HELEMENT he ) { return SAPI()->SciterDetachElement( he ); }
  inline SCDOM_RESULT SCAPI SciterDeleteElement(HELEMENT he) { return SAPI()->SciterDeleteElement(he); }
  inline SCDOM_RESULT SCAPI SciterSetTimer( HELEMENT he, UINT milliseconds, UINT_PTR timer_id ) { return SAPI()->SciterSetTimer(he,milliseconds,timer_id ); }
  inline SCDOM_RESULT SCAPI SciterDetachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI()->SciterDetachEventHandler(he,pep,tag ); }
  inline SCDOM_RESULT SCAPI SciterAttachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI()->SciterAttachEventHandler( he,pep,tag ); }
  inline SCDOM_RESULT SCAPI SciterWindowAttachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag, UINT subscription ) { return SAPI()->SciterWindowAttachEventHandler(hwndLayout,pep,tag,subscription ); }
  inline SCDOM_RESULT SCAPI SciterWindowDetachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI()->SciterWindowDetachEventHandler(hwndLayout,pep,tag ); }
  inline SCDOM_RESULT SCAPI SciterSendEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason, /*out*/ BOOL* handled) { return SAPI()->SciterSendEvent( he,appEventCode,heSource,reason,handled); }
  inline SCDOM_RESULT SCAPI SciterPostEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason) { return SAPI()->SciterPostEvent(he,appEventCode,heSource,reason); }
  inline SCDOM_RESULT SCAPI SciterFireEvent( const BEHAVIOR_EVENT_PARAMS* evt, BOOL post, BOOL *handled) { return SAPI()->SciterFireEvent( evt, post, handled ); }
  inline SCDOM_RESULT SCAPI SciterCallBehaviorMethod(HELEMENT he, struct METHOD_PARAMS* params) { return SAPI()->SciterCallBehaviorMethod(he,params); }
  inline SCDOM_RESULT SCAPI SciterRequestElementData( HELEMENT he, LPCWSTR url, UINT dataType, HELEMENT initiator ) { return SAPI()->SciterRequestElementData(he,url,dataType,initiator ); }
  inline SCDOM_RESULT SCAPI SciterHttpRequest( HELEMENT he, LPCWSTR url, UINT dataType, UINT requestType, struct REQUEST_PARAM* requestParams, UINT nParams) { return SAPI()->SciterHttpRequest(he,url,dataType,requestType,requestParams,nParams); }
  inline SCDOM_RESULT SCAPI SciterGetScrollInfo( HELEMENT he, LPPOINT scrollPos, LPRECT viewRect, LPSIZE contentSize ) { return SAPI()->SciterGetScrollInfo( he,scrollPos,viewRect,contentSize ); }
  inline SCDOM_RESULT SCAPI SciterSetScrollPos( HELEMENT he, POINT scrollPos, BOOL smooth ) { return SAPI()->SciterSetScrollPos( he,scrollPos,smooth ); }
  inline SCDOM_RESULT SCAPI SciterGetElementIntrinsicWidths( HELEMENT he, INT* pMinWidth, INT* pMaxWidth ) { return SAPI()->SciterGetElementIntrinsicWidths(he,pMinWidth,pMaxWidth ); }
  inline SCDOM_RESULT SCAPI SciterGetElementIntrinsicHeight( HELEMENT he, INT forWidth, INT* pHeight ) { return SAPI()->SciterGetElementIntrinsicHeight( he,forWidth,pHeight ); }
  inline SCDOM_RESULT SCAPI SciterIsElementVisible( HELEMENT he, BOOL* pVisible) { return SAPI()->SciterIsElementVisible( he,pVisible); }
  inline SCDOM_RESULT SCAPI SciterIsElementEnabled( HELEMENT he, BOOL* pEnabled ) { return SAPI()->SciterIsElementEnabled( he, pEnabled ); }
  inline SCDOM_RESULT SCAPI SciterSortElements( HELEMENT he, UINT firstIndex, UINT lastIndex, ELEMENT_COMPARATOR* cmpFunc, LPVOID cmpFuncParam ) { return SAPI()->SciterSortElements( he, firstIndex, lastIndex, cmpFunc, cmpFuncParam ); }
  inline SCDOM_RESULT SCAPI SciterSwapElements( HELEMENT he1, HELEMENT he2 ) { return SAPI()->SciterSwapElements( he1,he2 ); }
  inline SCDOM_RESULT SCAPI SciterTraverseUIEvent( UINT evt, LPVOID eventCtlStruct, BOOL* bOutProcessed ) { return SAPI()->SciterTraverseUIEvent( evt,eventCtlStruct,bOutProcessed ); }
  inline SCDOM_RESULT SCAPI SciterCallScriptingMethod( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) { return SAPI()->SciterCallScriptingMethod( he,name,argv,argc,retval ); }
  inline SCDOM_RESULT SCAPI SciterCallScriptingFunction( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) { return SAPI()->SciterCallScriptingFunction( he,name,argv,argc,retval ); }
  inline SCDOM_RESULT SCAPI SciterEvalElementScript( HELEMENT he, LPCWSTR script, UINT scriptLength, VALUE* retval ) { return SAPI()->SciterEvalElementScript( he, script, scriptLength, retval ); }
  inline SCDOM_RESULT SCAPI SciterAttachHwndToElement(HELEMENT he, HWINDOW hwnd) { return SAPI()->SciterAttachHwndToElement(he,hwnd); }
  inline SCDOM_RESULT SCAPI SciterControlGetType( HELEMENT he, /*CTL_TYPE*/ UINT *pType ) { return SAPI()->SciterControlGetType( he, pType ); }
  inline SCDOM_RESULT SCAPI SciterGetValue( HELEMENT he, VALUE* pval ) { return SAPI()->SciterGetValue( he,pval ); }
  inline SCDOM_RESULT SCAPI SciterSetValue( HELEMENT he, const VALUE* pval ) { return SAPI()->SciterSetValue( he, pval ); }
  inline SCDOM_RESULT SCAPI SciterGetExpando( HELEMENT he, VALUE* pval, BOOL forceCreation ) { return SAPI()->SciterGetExpando( he, pval, forceCreation ); }
  inline SCDOM_RESULT SCAPI SciterGetObject( HELEMENT he, tiscript_value* pval, BOOL forceCreation ) { return SAPI()->SciterGetObject( he, pval, forceCreation ); }
  inline SCDOM_RESULT SCAPI SciterGetElementNamespace(  HELEMENT he, tiscript_value* pval) { return SAPI()->SciterGetElementNamespace( he,pval); }
  inline SCDOM_RESULT SCAPI SciterGetHighlightedElement(HWINDOW hwnd, HELEMENT* phe) { return SAPI()->SciterGetHighlightedElement(hwnd, phe); }
  inline SCDOM_RESULT SCAPI SciterSetHighlightedElement(HWINDOW hwnd, HELEMENT he) { return SAPI()->SciterSetHighlightedElement(hwnd,he); }
  inline SCDOM_RESULT SCAPI SciterNodeAddRef(HNODE hn) { return SAPI()->SciterNodeAddRef(hn); }
  inline SCDOM_RESULT SCAPI SciterNodeRelease(HNODE hn) { return SAPI()->SciterNodeRelease(hn); }
  inline SCDOM_RESULT SCAPI SciterNodeCastFromElement(HELEMENT he, HNODE* phn) { return SAPI()->SciterNodeCastFromElement(he,phn); }
  inline SCDOM_RESULT SCAPI SciterNodeCastToElement(HNODE hn, HELEMENT* he) { return SAPI()->SciterNodeCastToElement(hn,he); }
  inline SCDOM_RESULT SCAPI SciterNodeFirstChild(HNODE hn, HNODE* phn) { return SAPI()->SciterNodeFirstChild(hn,phn); }
  inline SCDOM_RESULT SCAPI SciterNodeLastChild(HNODE hn, HNODE* phn) { return SAPI()->SciterNodeLastChild(hn, phn); }
  inline SCDOM_RESULT SCAPI SciterNodeNextSibling(HNODE hn, HNODE* phn) { return SAPI()->SciterNodeNextSibling(hn, phn); }
  inline SCDOM_RESULT SCAPI SciterNodePrevSibling(HNODE hn, HNODE* phn) { return SAPI()->SciterNodePrevSibling(hn,phn); }
  inline SCDOM_RESULT SCAPI SciterNodeParent(HNODE hnode, HELEMENT* pheParent) { return SAPI()->SciterNodeParent(hnode,pheParent) ; }
  inline SCDOM_RESULT SCAPI SciterNodeNthChild(HNODE hnode, UINT n, HNODE* phn) { return SAPI()->SciterNodeNthChild(hnode,n,phn); }
  inline SCDOM_RESULT SCAPI SciterNodeChildrenCount(HNODE hnode, UINT* pn) { return SAPI()->SciterNodeChildrenCount(hnode, pn); }
  inline SCDOM_RESULT SCAPI SciterNodeType(HNODE hnode, UINT* pNodeType /*NODE_TYPE*/) { return SAPI()->SciterNodeType(hnode,pNodeType); }
  inline SCDOM_RESULT SCAPI SciterNodeGetText(HNODE hnode, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI()->SciterNodeGetText(hnode,rcv,rcv_param); }
  inline SCDOM_RESULT SCAPI SciterNodeSetText(HNODE hnode, LPCWSTR text, UINT textLength) { return SAPI()->SciterNodeSetText(hnode,text,textLength); }
  inline SCDOM_RESULT SCAPI SciterNodeInsert(HNODE hnode, UINT where /*NODE_INS_TARGET*/, HNODE what) { return SAPI()->SciterNodeInsert(hnode,where,what); }
  inline SCDOM_RESULT SCAPI SciterNodeRemove(HNODE hnode, BOOL finalize) { return SAPI()->SciterNodeRemove(hnode,finalize); }
  inline SCDOM_RESULT SCAPI SciterCreateTextNode(LPCWSTR text, UINT textLength, HNODE* phnode) { return SAPI()->SciterCreateTextNode(text,textLength,phnode); }
  inline SCDOM_RESULT SCAPI SciterCreateCommentNode(LPCWSTR text, UINT textLength, HNODE* phnode) { return SAPI()->SciterCreateCommentNode(text,textLength,phnode); }

  inline HVM   SCAPI SciterGetVM( HWINDOW hwnd )  { return SAPI()->SciterGetVM(hwnd); }

  inline UINT SCAPI ValueInit ( VALUE* pval ) { return SAPI()->ValueInit(pval); }
  inline UINT SCAPI ValueClear ( VALUE* pval ) { return SAPI()->ValueClear(pval); }
  inline UINT SCAPI ValueCompare ( const VALUE* pval1, const VALUE* pval2 ) { return SAPI()->ValueCompare(pval1,pval2); }
  inline UINT SCAPI ValueCopy ( VALUE* pdst, const VALUE* psrc ) { return SAPI()->ValueCopy(pdst, psrc); }
  inline UINT SCAPI ValueIsolate ( VALUE* pdst ) { return SAPI()->ValueIsolate(pdst); }
  inline UINT SCAPI ValueType ( const VALUE* pval, UINT* pType, UINT* pUnits ) { return SAPI()->ValueType(pval,pType,pUnits); }
  inline UINT SCAPI ValueStringData ( const VALUE* pval, LPCWSTR* pChars, UINT* pNumChars ) { return SAPI()->ValueStringData(pval,pChars,pNumChars); }
  inline UINT SCAPI ValueStringDataSet ( VALUE* pval, LPCWSTR chars, UINT numChars, UINT units ) { return SAPI()->ValueStringDataSet(pval, chars, numChars, units); }
  inline UINT SCAPI ValueIntData ( const VALUE* pval, INT* pData ) { return SAPI()->ValueIntData ( pval, pData ); }
  inline UINT SCAPI ValueIntDataSet ( VALUE* pval, INT data, UINT type, UINT units ) { return SAPI()->ValueIntDataSet ( pval, data,type,units ); }
  inline UINT SCAPI ValueInt64Data ( const VALUE* pval, INT64* pData ) { return SAPI()->ValueInt64Data ( pval,pData ); }
  inline UINT SCAPI ValueInt64DataSet ( VALUE* pval, INT64 data, UINT type, UINT units ) { return SAPI()->ValueInt64DataSet ( pval,data,type,units ); }
  inline UINT SCAPI ValueFloatData ( const VALUE* pval, FLOAT_VALUE* pData ) { return SAPI()->ValueFloatData ( pval,pData ); }
  inline UINT SCAPI ValueFloatDataSet ( VALUE* pval, FLOAT_VALUE data, UINT type, UINT units ) { return SAPI()->ValueFloatDataSet ( pval,data,type,units ); }
  inline UINT SCAPI ValueBinaryData ( const VALUE* pval, LPCBYTE* pBytes, UINT* pnBytes ) { return SAPI()->ValueBinaryData ( pval,pBytes,pnBytes ); }
  inline UINT SCAPI ValueBinaryDataSet ( VALUE* pval, LPCBYTE pBytes, UINT nBytes, UINT type, UINT units ) { return SAPI()->ValueBinaryDataSet ( pval,pBytes,nBytes,type,units ); }
  inline UINT SCAPI ValueElementsCount ( const VALUE* pval, INT* pn) { return SAPI()->ValueElementsCount ( pval,pn); }
  inline UINT SCAPI ValueNthElementValue ( const VALUE* pval, INT n, VALUE* pretval) { return SAPI()->ValueNthElementValue ( pval, n, pretval); }
  inline UINT SCAPI ValueNthElementValueSet ( VALUE* pval, INT n, const VALUE* pval_to_set) { return SAPI()->ValueNthElementValueSet ( pval,n,pval_to_set); }
  inline UINT SCAPI ValueNthElementKey ( const VALUE* pval, INT n, VALUE* pretval) { return SAPI()->ValueNthElementKey ( pval,n,pretval); }
  inline UINT SCAPI ValueEnumElements ( const VALUE* pval, KeyValueCallback* penum, LPVOID param) { return SAPI()->ValueEnumElements (pval,penum,param); }
  inline UINT SCAPI ValueSetValueToKey ( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set) { return SAPI()->ValueSetValueToKey ( pval, pkey, pval_to_set); }
  inline UINT SCAPI ValueGetValueOfKey ( const VALUE* pval, const VALUE* pkey, VALUE* pretval) { return SAPI()->ValueGetValueOfKey ( pval, pkey,pretval); }
  inline UINT SCAPI ValueToString ( VALUE* pval, UINT how ) { return SAPI()->ValueToString ( pval,how ); }
  inline UINT SCAPI ValueFromString ( VALUE* pval, LPCWSTR str, UINT strLength, UINT how ) { return SAPI()->ValueFromString ( pval, str,strLength,how ); }
  inline UINT SCAPI ValueInvoke ( const VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url) { return SAPI()->ValueInvoke ( pval, pthis, argc, argv, pretval, url); }
  inline UINT SCAPI ValueNativeFunctorSet (VALUE* pval, NATIVE_FUNCTOR_INVOKE*  pinvoke, NATIVE_FUNCTOR_RELEASE* prelease, VOID* tag ) { return SAPI()->ValueNativeFunctorSet ( pval, pinvoke,prelease,tag); }
  inline BOOL SCAPI ValueIsNativeFunctor ( const VALUE* pval) { return SAPI()->ValueIsNativeFunctor (pval); }

  // conversion between script (managed) value and the VALUE ( com::variant alike thing )
  inline BOOL SCAPI Sciter_v2V(HVM vm, const tiscript_value script_value, VALUE* out_value, BOOL isolate) { return SAPI()->Sciter_v2V(vm,script_value,out_value, isolate); }
  inline BOOL SCAPI Sciter_V2v(HVM vm, const VALUE* value, tiscript_value* out_script_value) { return SAPI()->Sciter_V2v(vm,value,out_script_value); }
    
#ifdef WINDOWS
  inline BOOL SCAPI SciterCreateOnDirectXWindow(HWINDOW hwnd, IUnknown* pSwapChain) { return SAPI()->SciterCreateOnDirectXWindow(hwnd,pSwapChain); }
  inline BOOL SCAPI SciterRenderOnDirectXWindow(HWINDOW hwnd, HELEMENT elementToRenderOrNull, BOOL frontLayer) { return SAPI()->SciterRenderOnDirectXWindow(hwnd,elementToRenderOrNull,frontLayer); }
  inline BOOL SCAPI SciterRenderOnDirectXTexture(HWINDOW hwnd, HELEMENT elementToRenderOrNull, IUnknown* surface) { return SAPI()->SciterRenderOnDirectXTexture(hwnd,elementToRenderOrNull,surface); }
#endif


#endif
