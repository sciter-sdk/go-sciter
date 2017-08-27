#include <sciter-x.h>

// getting ISciterAPI reference:

#ifdef STATIC_LIB

    EXTERN_C ISciterAPI* SCAPI SciterAPI();

    ISciterAPI* SAPI( ISciterAPI* ext ) {
       static ISciterAPI* _api = NULL;
       if( ext ) _api = ext;
       if( !_api )
       {
          _api = SciterAPI();
          tiscript::ni( _api->TIScriptAPI() );
       }
       return _api;
    }

#elif defined(WINDOWS)

    ISciterAPI* SAPI( ISciterAPI* ext ) {
       static ISciterAPI* _api = NULL;
       if( ext ) _api = ext;
       if( !_api )
       {
          HMODULE hm = LoadLibrary( TEXT("sciter.dll") );
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
#if defined(__cplusplus)
              tiscript::ni( _api->TIScriptAPI() );
#endif
            } else {
              FreeLibrary(hm);
            }

          }
       }
       // assert(_api);
       if( !_api ) {
         MessageBox(NULL, TEXT("Sciter engine not found, quiting"),TEXT("Error"),MB_OK);
         exit(-1);
       }
       return _api;
    }

	HSARCHIVE SCAPI SciterOpenArchive (LPCBYTE archiveData, UINT archiveDataLength) { 
		return SAPI(NULL)->SciterOpenArchive (archiveData,archiveDataLength); 
	}

	BOOL SCAPI SciterGetArchiveItem (HSARCHIVE harc, LPCWSTR path, LPCBYTE* pdata, UINT* pdataLength){
		return SAPI(NULL)->SciterGetArchiveItem (harc,path,pdata,pdataLength); 
	}


#elif defined(OSX)

    //typedef ISciterAPI* SCAPI (*SciterAPI_ptr)();

    ISciterAPI* SAPI( ISciterAPI* ext )
    {
        static ISciterAPI* _api = NULL;
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
            #if defined(__cplusplus)
                tiscript::ni( _api->TIScriptAPI() );
            #endif
        }
        // assert(_api);
        return _api;
    }

#elif defined(LINUX)

    #include <libgen.h>
    #include <dlfcn.h>

    //typedef ISciterAPI* SCAPI (*SciterAPI_ptr)();

    ISciterAPI* SAPI( ISciterAPI* ext )
    {
        static ISciterAPI* _api = NULL;
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
                fprintf(stderr, "[%s] Unable to load library: %s\n", __FILE__, dlerror());
                const char* lookup_paths[] =
                {
                    "/" SCITER_DLL_NAME,
                    NULL
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
            #if defined(__cplusplus)
                tiscript::ni( _api->TIScriptAPI() );
            #endif
        }
        // assert(_api);
        return _api;
    }


#endif

  LPSciterGraphicsAPI gapi()
  {
    static LPSciterGraphicsAPI _gapi = NULL;
    if(!_gapi)
      _gapi = SAPI(NULL)->GetSciterGraphicsAPI();
    return _gapi;
  }

  LPSciterRequestAPI rapi()
  {
    static LPSciterRequestAPI _rapi = NULL;
    if(!_rapi)
      _rapi = SAPI(NULL)->GetSciterRequestAPI();
    return _rapi;
  }


  // defining "official" API functions:

    LPCWSTR SCAPI SciterClassName () { return SAPI(NULL)->SciterClassName(); }
    UINT    SCAPI SciterVersion (BOOL major) { return SAPI(NULL)->SciterVersion (major); }
    BOOL    SCAPI SciterDataReady (HWINDOW hwnd,LPCWSTR uri,LPCBYTE data, UINT dataLength) { return SAPI(NULL)->SciterDataReady (hwnd,uri,data,dataLength); }
    BOOL    SCAPI SciterDataReadyAsync (HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength, LPVOID requestId) { return SAPI(NULL)->SciterDataReadyAsync (hwnd,uri, data, dataLength, requestId); }
#ifdef WINDOWS
    LRESULT SCAPI SciterProc (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { return SAPI(NULL)->SciterProc (hwnd,msg,wParam,lParam); }
    LRESULT SCAPI SciterProcND (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* pbHandled) { return SAPI(NULL)->SciterProcND (hwnd,msg,wParam,lParam,pbHandled); }
#endif
    BOOL    SCAPI SciterLoadFile (HWINDOW hWndSciter, LPCWSTR filename) { return SAPI(NULL)->SciterLoadFile (hWndSciter,filename); }
    BOOL    SCAPI SciterLoadHtml (HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl) { return SAPI(NULL)->SciterLoadHtml (hWndSciter,html,htmlSize,baseUrl); }
    VOID    SCAPI SciterSetCallback (HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam) { SAPI(NULL)->SciterSetCallback (hWndSciter,cb,cbParam); }
    BOOL    SCAPI SciterSetMasterCSS (LPCBYTE utf8, UINT numBytes) { return SAPI(NULL)->SciterSetMasterCSS (utf8,numBytes); }
    BOOL    SCAPI SciterAppendMasterCSS (LPCBYTE utf8, UINT numBytes) { return SAPI(NULL)->SciterAppendMasterCSS (utf8,numBytes); }
    BOOL    SCAPI SciterSetCSS (HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType) { return SAPI(NULL)->SciterSetCSS (hWndSciter,utf8,numBytes,baseUrl,mediaType); }
    BOOL    SCAPI SciterSetMediaType (HWINDOW hWndSciter, LPCWSTR mediaType) { return SAPI(NULL)->SciterSetMediaType (hWndSciter,mediaType); }
    BOOL    SCAPI SciterSetMediaVars (HWINDOW hWndSciter, const SCITER_VALUE *mediaVars) { return SAPI(NULL)->SciterSetMediaVars (hWndSciter,mediaVars); }
    UINT    SCAPI SciterGetMinWidth (HWINDOW hWndSciter) { return SAPI(NULL)->SciterGetMinWidth (hWndSciter); }
    UINT    SCAPI SciterGetMinHeight (HWINDOW hWndSciter, UINT width) { return SAPI(NULL)->SciterGetMinHeight (hWndSciter,width); }
    BOOL    SCAPI SciterCall (HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval) { return SAPI(NULL)->SciterCall (hWnd,functionName, argc,argv,retval); }
    BOOL    SCAPI SciterEval ( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval) { return SAPI(NULL)->SciterEval ( hwnd, script, scriptLength, pretval); }
    VOID    SCAPI SciterUpdateWindow(HWINDOW hwnd) { SAPI(NULL)->SciterUpdateWindow(hwnd); }
#ifdef WINDOWS
    BOOL    SCAPI SciterTranslateMessage (MSG* lpMsg) { return SAPI(NULL)->SciterTranslateMessage (lpMsg); }
#endif
   BOOL    SCAPI SciterSetOption (HWINDOW hWnd, UINT option, UINT_PTR value ) { return SAPI(NULL)->SciterSetOption (hWnd,option,value ); }
   VOID    SCAPI SciterGetPPI (HWINDOW hWndSciter, UINT* px, UINT* py) { SAPI(NULL)->SciterGetPPI (hWndSciter,px,py); }
   BOOL    SCAPI SciterGetViewExpando ( HWINDOW hwnd, VALUE* pval ) { return SAPI(NULL)->SciterGetViewExpando ( hwnd, pval ); }
#ifdef WINDOWS
   BOOL    SCAPI SciterRenderD2D (HWINDOW hWndSciter, ID2D1RenderTarget* prt) { return SAPI(NULL)->SciterRenderD2D (hWndSciter,prt); }
   BOOL    SCAPI SciterD2DFactory (ID2D1Factory ** ppf) { return SAPI(NULL)->SciterD2DFactory (ppf); }
   BOOL    SCAPI SciterDWFactory (IDWriteFactory ** ppf) { return SAPI(NULL)->SciterDWFactory (ppf); }
#endif
   BOOL    SCAPI SciterGraphicsCaps (LPUINT pcaps) { return SAPI(NULL)->SciterGraphicsCaps (pcaps); }
   BOOL    SCAPI SciterSetHomeURL (HWINDOW hWndSciter, LPCWSTR baseUrl) { return SAPI(NULL)->SciterSetHomeURL (hWndSciter,baseUrl); }
#if defined(OSX)
   HWINDOW SCAPI SciterCreateNSView ( LPRECT frame ) { return SAPI(NULL)->SciterCreateNSView ( frame ); }
#endif
   HWINDOW SCAPI SciterCreateWindow ( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent) { return SAPI(NULL)->SciterCreateWindow (creationFlags,frame,delegate,delegateParam,parent); }

  SCDOM_RESULT SCAPI Sciter_UseElement(HELEMENT he) { return SAPI(NULL)->Sciter_UseElement(he); }
  SCDOM_RESULT SCAPI Sciter_UnuseElement(HELEMENT he) { return SAPI(NULL)->Sciter_UnuseElement(he); }
  SCDOM_RESULT SCAPI SciterGetRootElement(HWINDOW hwnd, HELEMENT *phe) { return SAPI(NULL)->SciterGetRootElement(hwnd, phe); }
  SCDOM_RESULT SCAPI SciterGetFocusElement(HWINDOW hwnd, HELEMENT *phe) { return SAPI(NULL)->SciterGetFocusElement(hwnd, phe); }
  SCDOM_RESULT SCAPI SciterFindElement(HWINDOW hwnd, POINT pt, HELEMENT* phe) { return SAPI(NULL)->SciterFindElement(hwnd,pt,phe); }
  SCDOM_RESULT SCAPI SciterGetChildrenCount(HELEMENT he, UINT* count) { return SAPI(NULL)->SciterGetChildrenCount(he, count); }
  SCDOM_RESULT SCAPI SciterGetNthChild(HELEMENT he, UINT n, HELEMENT* phe) { return SAPI(NULL)->SciterGetNthChild(he,n,phe); }
  SCDOM_RESULT SCAPI SciterGetParentElement(HELEMENT he, HELEMENT* p_parent_he) { return SAPI(NULL)->SciterGetParentElement(he,p_parent_he); }
  SCDOM_RESULT SCAPI SciterGetElementHtmlCB(HELEMENT he, BOOL outer, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetElementHtmlCB( he, outer, rcv, rcv_param); }
  SCDOM_RESULT SCAPI SciterGetElementTextCB(HELEMENT he, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetElementTextCB(he, rcv, rcv_param); }
  SCDOM_RESULT SCAPI SciterSetElementText(HELEMENT he, LPCWSTR utf16, UINT length) { return SAPI(NULL)->SciterSetElementText(he, utf16, length); }
  SCDOM_RESULT SCAPI SciterGetAttributeCount(HELEMENT he, LPUINT p_count) { return SAPI(NULL)->SciterGetAttributeCount(he, p_count); }
  SCDOM_RESULT SCAPI SciterGetNthAttributeNameCB(HELEMENT he, UINT n, LPCSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetNthAttributeNameCB(he,n,rcv,rcv_param); }
  SCDOM_RESULT SCAPI SciterGetNthAttributeValueCB(HELEMENT he, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetNthAttributeValueCB(he, n, rcv, rcv_param); }
  SCDOM_RESULT SCAPI SciterGetAttributeByNameCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetAttributeByNameCB(he,name,rcv,rcv_param); }
  SCDOM_RESULT SCAPI SciterSetAttributeByName(HELEMENT he, LPCSTR name, LPCWSTR value) { return SAPI(NULL)->SciterSetAttributeByName(he,name,value); }
  SCDOM_RESULT SCAPI SciterClearAttributes(HELEMENT he) { return SAPI(NULL)->SciterClearAttributes(he); }
  SCDOM_RESULT SCAPI SciterGetElementIndex(HELEMENT he, LPUINT p_index) { return SAPI(NULL)->SciterGetElementIndex(he,p_index); }
  SCDOM_RESULT SCAPI SciterGetElementType(HELEMENT he, LPCSTR* p_type) { return SAPI(NULL)->SciterGetElementType(he,p_type); }
  SCDOM_RESULT SCAPI SciterGetElementTypeCB(HELEMENT he, LPCSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetElementTypeCB(he,rcv,rcv_param); }
  SCDOM_RESULT SCAPI SciterGetStyleAttributeCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterGetStyleAttributeCB(he,name,rcv,rcv_param); }
  SCDOM_RESULT SCAPI SciterSetStyleAttribute(HELEMENT he, LPCSTR name, LPCWSTR value) { return SAPI(NULL)->SciterSetStyleAttribute(he,name,value); }
  SCDOM_RESULT SCAPI SciterGetElementLocation(HELEMENT he, LPRECT p_location, UINT areas /*ELEMENT_AREAS*/) { return SAPI(NULL)->SciterGetElementLocation(he,p_location,areas); }
  SCDOM_RESULT SCAPI SciterScrollToView(HELEMENT he, UINT SciterScrollFlags) { return SAPI(NULL)->SciterScrollToView(he,SciterScrollFlags); }
  SCDOM_RESULT SCAPI SciterUpdateElement(HELEMENT he, BOOL andForceRender) { return SAPI(NULL)->SciterUpdateElement(he,andForceRender); }
  SCDOM_RESULT SCAPI SciterRefreshElementArea(HELEMENT he, RECT rc) { return SAPI(NULL)->SciterRefreshElementArea(he,rc); }
  SCDOM_RESULT SCAPI SciterSetCapture(HELEMENT he) { return SAPI(NULL)->SciterSetCapture(he); }
  SCDOM_RESULT SCAPI SciterReleaseCapture(HELEMENT he) { return SAPI(NULL)->SciterReleaseCapture(he); }
  SCDOM_RESULT SCAPI SciterGetElementHwnd(HELEMENT he, HWINDOW* p_hwnd, BOOL rootWindow) { return SAPI(NULL)->SciterGetElementHwnd(he,p_hwnd,rootWindow); }
  SCDOM_RESULT SCAPI SciterCombineURL(HELEMENT he, LPWSTR szUrlBuffer, UINT UrlBufferSize) { return SAPI(NULL)->SciterCombineURL(he,szUrlBuffer,UrlBufferSize); }
  SCDOM_RESULT SCAPI SciterSelectElements(HELEMENT  he, LPCSTR    CSS_selectors, SciterElementCallback* callback, LPVOID param) { return SAPI(NULL)->SciterSelectElements(he,CSS_selectors,callback,param); }
  SCDOM_RESULT SCAPI SciterSelectElementsW(HELEMENT  he, LPCWSTR   CSS_selectors, SciterElementCallback* callback, LPVOID param) { return SAPI(NULL)->SciterSelectElementsW(he,CSS_selectors,callback,param); }
  SCDOM_RESULT SCAPI SciterSelectParent(HELEMENT  he, LPCSTR    selector, UINT      depth, HELEMENT* heFound) { return SAPI(NULL)->SciterSelectParent(he,selector,depth,heFound); }
  SCDOM_RESULT SCAPI SciterSelectParentW(HELEMENT  he, LPCWSTR   selector, UINT      depth, HELEMENT* heFound) { return SAPI(NULL)->SciterSelectParentW(he,selector,depth,heFound); }
  SCDOM_RESULT SCAPI SciterSetElementHtml(HELEMENT he, const BYTE* html, UINT htmlLength, UINT where) { return SAPI(NULL)->SciterSetElementHtml(he,html,htmlLength,where); }
  SCDOM_RESULT SCAPI SciterGetElementUID(HELEMENT he, UINT* puid) { return SAPI(NULL)->SciterGetElementUID(he,puid); }
  SCDOM_RESULT SCAPI SciterGetElementByUID(HWINDOW hwnd, UINT uid, HELEMENT* phe) { return SAPI(NULL)->SciterGetElementByUID(hwnd,uid,phe); }
  SCDOM_RESULT SCAPI SciterShowPopup(HELEMENT hePopup, HELEMENT heAnchor, UINT placement) { return SAPI(NULL)->SciterShowPopup(hePopup,heAnchor,placement); }
  SCDOM_RESULT SCAPI SciterShowPopupAt(HELEMENT hePopup, POINT pos, BOOL animate) { return SAPI(NULL)->SciterShowPopupAt(hePopup,pos,animate); }
  SCDOM_RESULT SCAPI SciterHidePopup(HELEMENT he) { return SAPI(NULL)->SciterHidePopup(he); }
  SCDOM_RESULT SCAPI SciterGetElementState( HELEMENT he, UINT* pstateBits) { return SAPI(NULL)->SciterGetElementState(he,pstateBits); }
  SCDOM_RESULT SCAPI SciterSetElementState( HELEMENT he, UINT stateBitsToSet, UINT stateBitsToClear, BOOL updateView) { return SAPI(NULL)->SciterSetElementState(he,stateBitsToSet,stateBitsToClear,updateView); }
  SCDOM_RESULT SCAPI SciterCreateElement( LPCSTR tagname, LPCWSTR textOrNull, /*out*/ HELEMENT *phe ) { return SAPI(NULL)->SciterCreateElement(tagname,textOrNull,phe ); }
  SCDOM_RESULT SCAPI SciterCloneElement( HELEMENT he, /*out*/ HELEMENT *phe ) { return SAPI(NULL)->SciterCloneElement(he,phe ); }
  SCDOM_RESULT SCAPI SciterInsertElement( HELEMENT he, HELEMENT hparent, UINT index ) { return SAPI(NULL)->SciterInsertElement(he,hparent,index ); }
  SCDOM_RESULT SCAPI SciterDetachElement( HELEMENT he ) { return SAPI(NULL)->SciterDetachElement( he ); }
  SCDOM_RESULT SCAPI SciterDeleteElement(HELEMENT he) { return SAPI(NULL)->SciterDeleteElement(he); }
  SCDOM_RESULT SCAPI SciterSetTimer( HELEMENT he, UINT milliseconds, UINT_PTR timer_id ) { return SAPI(NULL)->SciterSetTimer(he,milliseconds,timer_id ); }
  SCDOM_RESULT SCAPI SciterDetachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI(NULL)->SciterDetachEventHandler(he,pep,tag ); }
  SCDOM_RESULT SCAPI SciterAttachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI(NULL)->SciterAttachEventHandler( he,pep,tag ); }
  SCDOM_RESULT SCAPI SciterWindowAttachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag, UINT subscription ) { return SAPI(NULL)->SciterWindowAttachEventHandler(hwndLayout,pep,tag,subscription ); }
  SCDOM_RESULT SCAPI SciterWindowDetachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag ) { return SAPI(NULL)->SciterWindowDetachEventHandler(hwndLayout,pep,tag ); }
  SCDOM_RESULT SCAPI SciterSendEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason, /*out*/ BOOL* handled) { return SAPI(NULL)->SciterSendEvent( he,appEventCode,heSource,reason,handled); }
  SCDOM_RESULT SCAPI SciterPostEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason) { return SAPI(NULL)->SciterPostEvent(he,appEventCode,heSource,reason); }
  SCDOM_RESULT SCAPI SciterFireEvent( const struct BEHAVIOR_EVENT_PARAMS* evt, BOOL post, BOOL *handled) { return SAPI(NULL)->SciterFireEvent( evt, post, handled ); }
  SCDOM_RESULT SCAPI SciterCallBehaviorMethod(HELEMENT he, struct METHOD_PARAMS* params) { return SAPI(NULL)->SciterCallBehaviorMethod(he,params); }
  SCDOM_RESULT SCAPI SciterRequestElementData( HELEMENT he, LPCWSTR url, UINT dataType, HELEMENT initiator ) { return SAPI(NULL)->SciterRequestElementData(he,url,dataType,initiator ); }
  SCDOM_RESULT SCAPI SciterHttpRequest( HELEMENT he, LPCWSTR url, UINT dataType, UINT requestType, struct REQUEST_PARAM* requestParams, UINT nParams) { return SAPI(NULL)->SciterHttpRequest(he,url,dataType,requestType,requestParams,nParams); }
  SCDOM_RESULT SCAPI SciterGetScrollInfo( HELEMENT he, LPPOINT scrollPos, LPRECT viewRect, LPSIZE contentSize ) { return SAPI(NULL)->SciterGetScrollInfo( he,scrollPos,viewRect,contentSize ); }
  SCDOM_RESULT SCAPI SciterSetScrollPos( HELEMENT he, POINT scrollPos, BOOL smooth ) { return SAPI(NULL)->SciterSetScrollPos( he,scrollPos,smooth ); }
  SCDOM_RESULT SCAPI SciterGetElementIntrinsicWidths( HELEMENT he, INT* pMinWidth, INT* pMaxWidth ) { return SAPI(NULL)->SciterGetElementIntrinsicWidths(he,pMinWidth,pMaxWidth ); }
  SCDOM_RESULT SCAPI SciterGetElementIntrinsicHeight( HELEMENT he, INT forWidth, INT* pHeight ) { return SAPI(NULL)->SciterGetElementIntrinsicHeight( he,forWidth,pHeight ); }
  SCDOM_RESULT SCAPI SciterIsElementVisible( HELEMENT he, BOOL* pVisible) { return SAPI(NULL)->SciterIsElementVisible( he,pVisible); }
  SCDOM_RESULT SCAPI SciterIsElementEnabled( HELEMENT he, BOOL* pEnabled ) { return SAPI(NULL)->SciterIsElementEnabled( he, pEnabled ); }
  SCDOM_RESULT SCAPI SciterSortElements( HELEMENT he, UINT firstIndex, UINT lastIndex, ELEMENT_COMPARATOR* cmpFunc, LPVOID cmpFuncParam ) { return SAPI(NULL)->SciterSortElements( he, firstIndex, lastIndex, cmpFunc, cmpFuncParam ); }
  SCDOM_RESULT SCAPI SciterSwapElements( HELEMENT he1, HELEMENT he2 ) { return SAPI(NULL)->SciterSwapElements( he1,he2 ); }
  SCDOM_RESULT SCAPI SciterTraverseUIEvent( UINT evt, LPVOID eventCtlStruct, BOOL* bOutProcessed ) { return SAPI(NULL)->SciterTraverseUIEvent( evt,eventCtlStruct,bOutProcessed ); }
  SCDOM_RESULT SCAPI SciterCallScriptingMethod( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) { return SAPI(NULL)->SciterCallScriptingMethod( he,name,argv,argc,retval ); }
  SCDOM_RESULT SCAPI SciterCallScriptingFunction( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) { return SAPI(NULL)->SciterCallScriptingFunction( he,name,argv,argc,retval ); }
  SCDOM_RESULT SCAPI SciterEvalElementScript( HELEMENT he, LPCWSTR script, UINT scriptLength, VALUE* retval ) { return SAPI(NULL)->SciterEvalElementScript( he, script, scriptLength, retval ); }
  SCDOM_RESULT SCAPI SciterAttachHwndToElement(HELEMENT he, HWINDOW hwnd) { return SAPI(NULL)->SciterAttachHwndToElement(he,hwnd); }
  SCDOM_RESULT SCAPI SciterControlGetType( HELEMENT he, /*CTL_TYPE*/ UINT *pType ) { return SAPI(NULL)->SciterControlGetType( he, pType ); }
  SCDOM_RESULT SCAPI SciterGetValue( HELEMENT he, VALUE* pval ) { return SAPI(NULL)->SciterGetValue( he,pval ); }
  SCDOM_RESULT SCAPI SciterSetValue( HELEMENT he, const VALUE* pval ) { return SAPI(NULL)->SciterSetValue( he, pval ); }
  SCDOM_RESULT SCAPI SciterGetExpando( HELEMENT he, VALUE* pval, BOOL forceCreation ) { return SAPI(NULL)->SciterGetExpando( he, pval, forceCreation ); }
  SCDOM_RESULT SCAPI SciterGetObject( HELEMENT he, tiscript_value* pval, BOOL forceCreation ) { return SAPI(NULL)->SciterGetObject( he, pval, forceCreation ); }
  SCDOM_RESULT SCAPI SciterGetElementNamespace(  HELEMENT he, tiscript_value* pval) { return SAPI(NULL)->SciterGetElementNamespace( he,pval); }
  SCDOM_RESULT SCAPI SciterGetHighlightedElement(HWINDOW hwnd, HELEMENT* phe) { return SAPI(NULL)->SciterGetHighlightedElement(hwnd, phe); }
  SCDOM_RESULT SCAPI SciterSetHighlightedElement(HWINDOW hwnd, HELEMENT he) { return SAPI(NULL)->SciterSetHighlightedElement(hwnd,he); }
  SCDOM_RESULT SCAPI SciterNodeAddRef(HNODE hn) { return SAPI(NULL)->SciterNodeAddRef(hn); }
  SCDOM_RESULT SCAPI SciterNodeRelease(HNODE hn) { return SAPI(NULL)->SciterNodeRelease(hn); }
  SCDOM_RESULT SCAPI SciterNodeCastFromElement(HELEMENT he, HNODE* phn) { return SAPI(NULL)->SciterNodeCastFromElement(he,phn); }
  SCDOM_RESULT SCAPI SciterNodeCastToElement(HNODE hn, HELEMENT* he) { return SAPI(NULL)->SciterNodeCastToElement(hn,he); }
  SCDOM_RESULT SCAPI SciterNodeFirstChild(HNODE hn, HNODE* phn) { return SAPI(NULL)->SciterNodeFirstChild(hn,phn); }
  SCDOM_RESULT SCAPI SciterNodeLastChild(HNODE hn, HNODE* phn) { return SAPI(NULL)->SciterNodeLastChild(hn, phn); }
  SCDOM_RESULT SCAPI SciterNodeNextSibling(HNODE hn, HNODE* phn) { return SAPI(NULL)->SciterNodeNextSibling(hn, phn); }
  SCDOM_RESULT SCAPI SciterNodePrevSibling(HNODE hn, HNODE* phn) { return SAPI(NULL)->SciterNodePrevSibling(hn,phn); }
  SCDOM_RESULT SCAPI SciterNodeParent(HNODE hnode, HELEMENT* pheParent) { return SAPI(NULL)->SciterNodeParent(hnode,pheParent) ; }
  SCDOM_RESULT SCAPI SciterNodeNthChild(HNODE hnode, UINT n, HNODE* phn) { return SAPI(NULL)->SciterNodeNthChild(hnode,n,phn); }
  SCDOM_RESULT SCAPI SciterNodeChildrenCount(HNODE hnode, UINT* pn) { return SAPI(NULL)->SciterNodeChildrenCount(hnode, pn); }
  SCDOM_RESULT SCAPI SciterNodeType(HNODE hnode, UINT* pNodeType /*NODE_TYPE*/) { return SAPI(NULL)->SciterNodeType(hnode,pNodeType); }
  SCDOM_RESULT SCAPI SciterNodeGetText(HNODE hnode, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) { return SAPI(NULL)->SciterNodeGetText(hnode,rcv,rcv_param); }
  SCDOM_RESULT SCAPI SciterNodeSetText(HNODE hnode, LPCWSTR text, UINT textLength) { return SAPI(NULL)->SciterNodeSetText(hnode,text,textLength); }
  SCDOM_RESULT SCAPI SciterNodeInsert(HNODE hnode, UINT where /*NODE_INS_TARGET*/, HNODE what) { return SAPI(NULL)->SciterNodeInsert(hnode,where,what); }
  SCDOM_RESULT SCAPI SciterNodeRemove(HNODE hnode, BOOL finalize) { return SAPI(NULL)->SciterNodeRemove(hnode,finalize); }
  SCDOM_RESULT SCAPI SciterCreateTextNode(LPCWSTR text, UINT textLength, HNODE* phnode) { return SAPI(NULL)->SciterCreateTextNode(text,textLength,phnode); }
  SCDOM_RESULT SCAPI SciterCreateCommentNode(LPCWSTR text, UINT textLength, HNODE* phnode) { return SAPI(NULL)->SciterCreateCommentNode(text,textLength,phnode); }

  HVM   SCAPI SciterGetVM( HWINDOW hwnd )  { return SAPI(NULL)->SciterGetVM(hwnd); }

  UINT SCAPI ValueInit ( VALUE* pval ) { return SAPI(NULL)->ValueInit(pval); }
  UINT SCAPI ValueClear ( VALUE* pval ) { return SAPI(NULL)->ValueClear(pval); }
  UINT SCAPI ValueCompare ( const VALUE* pval1, const VALUE* pval2 ) { return SAPI(NULL)->ValueCompare(pval1,pval2); }
  UINT SCAPI ValueCopy ( VALUE* pdst, const VALUE* psrc ) { return SAPI(NULL)->ValueCopy(pdst, psrc); }
  UINT SCAPI ValueIsolate ( VALUE* pdst ) { return SAPI(NULL)->ValueIsolate(pdst); }
  UINT SCAPI ValueType ( const VALUE* pval, UINT* pType, UINT* pUnits ) { return SAPI(NULL)->ValueType(pval,pType,pUnits); }
  UINT SCAPI ValueStringData ( const VALUE* pval, LPCWSTR* pChars, UINT* pNumChars ) { return SAPI(NULL)->ValueStringData(pval,pChars,pNumChars); }
  UINT SCAPI ValueStringDataSet ( VALUE* pval, LPCWSTR chars, UINT numChars, UINT units ) { return SAPI(NULL)->ValueStringDataSet(pval, chars, numChars, units); }
  UINT SCAPI ValueIntData ( const VALUE* pval, INT* pData ) { return SAPI(NULL)->ValueIntData ( pval, pData ); }
  UINT SCAPI ValueIntDataSet ( VALUE* pval, INT data, UINT type, UINT units ) { return SAPI(NULL)->ValueIntDataSet ( pval, data,type,units ); }
  UINT SCAPI ValueInt64Data ( const VALUE* pval, INT64* pData ) { return SAPI(NULL)->ValueInt64Data ( pval,pData ); }
  UINT SCAPI ValueInt64DataSet ( VALUE* pval, INT64 data, UINT type, UINT units ) { return SAPI(NULL)->ValueInt64DataSet ( pval,data,type,units ); }
  UINT SCAPI ValueFloatData ( const VALUE* pval, FLOAT_VALUE* pData ) { return SAPI(NULL)->ValueFloatData ( pval,pData ); }
  UINT SCAPI ValueFloatDataSet ( VALUE* pval, FLOAT_VALUE data, UINT type, UINT units ) { return SAPI(NULL)->ValueFloatDataSet ( pval,data,type,units ); }
  UINT SCAPI ValueBinaryData ( const VALUE* pval, LPCBYTE* pBytes, UINT* pnBytes ) { return SAPI(NULL)->ValueBinaryData ( pval,pBytes,pnBytes ); }
  UINT SCAPI ValueBinaryDataSet ( VALUE* pval, LPCBYTE pBytes, UINT nBytes, UINT type, UINT units ) { return SAPI(NULL)->ValueBinaryDataSet ( pval,pBytes,nBytes,type,units ); }
  UINT SCAPI ValueElementsCount ( const VALUE* pval, INT* pn) { return SAPI(NULL)->ValueElementsCount ( pval,pn); }
  UINT SCAPI ValueNthElementValue ( const VALUE* pval, INT n, VALUE* pretval) { return SAPI(NULL)->ValueNthElementValue ( pval, n, pretval); }
  UINT SCAPI ValueNthElementValueSet ( VALUE* pval, INT n, const VALUE* pval_to_set) { return SAPI(NULL)->ValueNthElementValueSet ( pval,n,pval_to_set); }
  UINT SCAPI ValueNthElementKey ( const VALUE* pval, INT n, VALUE* pretval) { return SAPI(NULL)->ValueNthElementKey ( pval,n,pretval); }
  UINT SCAPI ValueEnumElements ( const VALUE* pval, KeyValueCallback* penum, LPVOID param) { return SAPI(NULL)->ValueEnumElements (pval,penum,param); }
  UINT SCAPI ValueSetValueToKey ( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set) { return SAPI(NULL)->ValueSetValueToKey ( pval, pkey, pval_to_set); }
  UINT SCAPI ValueGetValueOfKey ( const VALUE* pval, const VALUE* pkey, VALUE* pretval) { return SAPI(NULL)->ValueGetValueOfKey ( pval, pkey,pretval); }
  UINT SCAPI ValueToString ( VALUE* pval, UINT how ) { return SAPI(NULL)->ValueToString ( pval,how ); }
  UINT SCAPI ValueFromString ( VALUE* pval, LPCWSTR str, UINT strLength, UINT how ) { return SAPI(NULL)->ValueFromString ( pval, str,strLength,how ); }
  UINT SCAPI ValueInvoke ( const VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url) { return SAPI(NULL)->ValueInvoke ( pval, pthis, argc, argv, pretval, url); }
  UINT SCAPI ValueNativeFunctorSet (VALUE* pval, NATIVE_FUNCTOR_INVOKE*  pinvoke, NATIVE_FUNCTOR_RELEASE* prelease, VOID* tag ) { return SAPI(NULL)->ValueNativeFunctorSet ( pval, pinvoke,prelease,tag); }
  BOOL SCAPI ValueIsNativeFunctor ( const VALUE* pval) { return SAPI(NULL)->ValueIsNativeFunctor (pval); }

  // conversion between script (managed) value and the VALUE ( com::variant alike thing )
  BOOL SCAPI Sciter_v2V(HVM vm, const tiscript_value script_value, VALUE* out_value, BOOL isolate) { return SAPI(NULL)->Sciter_v2V(vm,script_value,out_value, isolate); }
  BOOL SCAPI Sciter_V2v(HVM vm, const VALUE* value, tiscript_value* out_script_value) { return SAPI(NULL)->Sciter_V2v(vm,value,out_script_value); }

  BOOL SCAPI SciterProcX(HWINDOW hwnd, SCITER_X_MSG* pMsg) { return SAPI(NULL)->SciterProcX(hwnd, pMsg); }
