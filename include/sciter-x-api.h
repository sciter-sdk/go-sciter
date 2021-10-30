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
#include "sciter-x-msg.h"
#include "value.h"

#if !defined(WINDOWS)
  #include <stdlib.h>
  #include <unistd.h>
  #if defined(OSX)
    #include <libproc.h>
  #endif
  #if defined(ANDROID)
    #include <dlfcn.h>
  #endif
#endif

#if defined(OSX)
  #include <dlfcn.h>
#endif

#ifdef __cplusplus
  #include <cstddef>
#endif

struct SciterGraphicsAPI;
struct SCITER_X_MSG;

#ifdef WINDOWLESS
  #define SCITER_API_VERSION 0x10009
#else 
  #define SCITER_API_VERSION 9
#endif

typedef struct _ISciterAPI {

  UINT    version; // API_VERSION

  LPCWSTR SCFN( SciterClassName )(void);
  UINT    SCFN( SciterVersion )(SBOOL major);
  SBOOL    SCFN( SciterDataReady )(HWINDOW hwnd,LPCWSTR uri,LPCBYTE data, UINT dataLength);
  SBOOL    SCFN( SciterDataReadyAsync )(HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength, LPVOID requestId);
#if defined(WINDOWS)
  LRESULT SCFN( SciterProc )(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  LRESULT SCFN( SciterProcND )(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, SBOOL* pbHandled);
#else 
  LPVOID   SciterProc; // NULL
  LPVOID   SciterProcND; // NULL
#endif
  SBOOL    SCFN( SciterLoadFile )(HWINDOW hWndSciter, LPCWSTR filename);

  SBOOL    SCFN( SciterLoadHtml )(HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl);
  VOID    SCFN( SciterSetCallback )(HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam);
  SBOOL    SCFN( SciterSetMasterCSS )(LPCBYTE utf8, UINT numBytes);
  SBOOL    SCFN( SciterAppendMasterCSS )(LPCBYTE utf8, UINT numBytes);
  SBOOL    SCFN( SciterSetCSS )(HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType);
  SBOOL    SCFN( SciterSetMediaType )(HWINDOW hWndSciter, LPCWSTR mediaType);
  SBOOL    SCFN( SciterSetMediaVars )(HWINDOW hWndSciter, const SCITER_VALUE *mediaVars);
  UINT    SCFN( SciterGetMinWidth )(HWINDOW hWndSciter);
  UINT    SCFN( SciterGetMinHeight )(HWINDOW hWndSciter, UINT width);
  SBOOL    SCFN( SciterCall )(HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval);
  SBOOL    SCFN( SciterEval )( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval);
  VOID    SCFN( SciterUpdateWindow)(HWINDOW hwnd);
#if defined(WINDOWS)
  SBOOL    SCFN(SciterTranslateMessage)(MSG* lpMsg);
#else 
  LPVOID   SciterTranslateMessage; // NULL
#endif
  SBOOL    SCFN( SciterSetOption )(HWINDOW hWnd, UINT option, UINT_PTR value );
  VOID    SCFN( SciterGetPPI )(HWINDOW hWndSciter, UINT* px, UINT* py);
  SBOOL    SCFN( SciterGetViewExpando )( HWINDOW hwnd, VALUE* pval );
#if defined(WINDOWS)
  SBOOL    SCFN( SciterRenderD2D )(HWINDOW hWndSciter, IUnknown* /*ID2D1RenderTarget**/ prt);
  SBOOL    SCFN( SciterD2DFactory )(IUnknown** /*ID2D1Factory ***/ ppf);
  SBOOL    SCFN( SciterDWFactory )(IUnknown** /*IDWriteFactory ***/ ppf);
#else 
  LPVOID   SciterRenderD2D;
  LPVOID   SciterD2DFactory;
  LPVOID   SciterDWFactory;
#endif
  SBOOL    SCFN( SciterGraphicsCaps )(LPUINT pcaps);
  SBOOL    SCFN( SciterSetHomeURL )(HWINDOW hWndSciter, LPCWSTR baseUrl);
#if defined(OSX)
  HWINDOW SCFN( SciterCreateNSView )( LPRECT frame ); // returns NSView*
#else 
  LPVOID SciterCreateNSView; // NULL
#endif
#if defined(LINUX)
  HWINDOW SCFN( SciterCreateWidget )( LPRECT frame ); // returns GtkWidget
#else 
  LPVOID SciterCreateWidget; // NULL
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
  SCDOM_RESULT SCFN( SciterGetElementHtmlCB)(HELEMENT he, SBOOL outer, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param);
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
  SCDOM_RESULT SCFN( SciterUpdateElement)(HELEMENT he, SBOOL andForceRender);
  SCDOM_RESULT SCFN( SciterRefreshElementArea)(HELEMENT he, RECT rc);
  SCDOM_RESULT SCFN( SciterSetCapture)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterReleaseCapture)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetElementHwnd)(HELEMENT he, HWINDOW* p_hwnd, SBOOL rootWindow);
  SCDOM_RESULT SCFN( SciterCombineURL)(HELEMENT he, LPWSTR szUrlBuffer, UINT UrlBufferSize);
  SCDOM_RESULT SCFN( SciterSelectElements)(HELEMENT  he, LPCSTR    CSS_selectors, SciterElementCallback* callback, LPVOID param);
  SCDOM_RESULT SCFN( SciterSelectElementsW)(HELEMENT  he, LPCWSTR   CSS_selectors, SciterElementCallback* callback, LPVOID param);
  SCDOM_RESULT SCFN( SciterSelectParent)(HELEMENT  he, LPCSTR    selector, UINT      depth, HELEMENT* heFound);
  SCDOM_RESULT SCFN( SciterSelectParentW)(HELEMENT  he, LPCWSTR   selector, UINT      depth, HELEMENT* heFound);
  SCDOM_RESULT SCFN( SciterSetElementHtml)(HELEMENT he, const BYTE* html, UINT htmlLength, UINT where);
  SCDOM_RESULT SCFN( SciterGetElementUID)(HELEMENT he, UINT* puid);
  SCDOM_RESULT SCFN( SciterGetElementByUID)(HWINDOW hwnd, UINT uid, HELEMENT* phe);
  SCDOM_RESULT SCFN( SciterShowPopup)(HELEMENT hePopup, HELEMENT heAnchor, UINT placement);
  SCDOM_RESULT SCFN( SciterShowPopupAt)(HELEMENT hePopup, POINT pos, UINT placement);
  SCDOM_RESULT SCFN( SciterHidePopup)(HELEMENT he);
  SCDOM_RESULT SCFN( SciterGetElementState)( HELEMENT he, UINT* pstateBits);
  SCDOM_RESULT SCFN( SciterSetElementState)( HELEMENT he, UINT stateBitsToSet, UINT stateBitsToClear, SBOOL updateView);
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
  SCDOM_RESULT SCFN( SciterSendEvent)( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT_PTR reason, /*out*/ SBOOL* handled);
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
  SCDOM_RESULT SCFN( SciterSetScrollPos)( HELEMENT he, POINT scrollPos, SBOOL smooth );
  SCDOM_RESULT SCFN( SciterGetElementIntrinsicWidths)( HELEMENT he, INT* pMinWidth, INT* pMaxWidth );
  SCDOM_RESULT SCFN( SciterGetElementIntrinsicHeight)( HELEMENT he, INT forWidth, INT* pHeight );
  SCDOM_RESULT SCFN( SciterIsElementVisible)( HELEMENT he, SBOOL* pVisible);
  SCDOM_RESULT SCFN( SciterIsElementEnabled)( HELEMENT he, SBOOL* pEnabled );
  SCDOM_RESULT SCFN( SciterSortElements)( HELEMENT he, UINT firstIndex, UINT lastIndex, ELEMENT_COMPARATOR* cmpFunc, LPVOID cmpFuncParam );
  SCDOM_RESULT SCFN( SciterSwapElements)( HELEMENT he1, HELEMENT he2 );
  SCDOM_RESULT SCFN( SciterTraverseUIEvent)( UINT evt, LPVOID eventCtlStruct, SBOOL* bOutProcessed );
  SCDOM_RESULT SCFN( SciterCallScriptingMethod)( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval );
  SCDOM_RESULT SCFN( SciterCallScriptingFunction)( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval );
  SCDOM_RESULT SCFN( SciterEvalElementScript)( HELEMENT he, LPCWSTR script, UINT scriptLength, VALUE* retval );
  SCDOM_RESULT SCFN( SciterAttachHwndToElement)(HELEMENT he, HWINDOW hwnd);
  SCDOM_RESULT SCFN( SciterControlGetType)( HELEMENT he, /*CTL_TYPE*/ UINT *pType );
  SCDOM_RESULT SCFN( SciterGetValue)( HELEMENT he, VALUE* pval );
  SCDOM_RESULT SCFN( SciterSetValue)( HELEMENT he, const VALUE* pval );
  SCDOM_RESULT SCFN( SciterGetExpando)( HELEMENT he, VALUE* pval, SBOOL forceCreation );
  SCDOM_RESULT SCFN( SciterGetObject)( HELEMENT he, void* pval, SBOOL forceCreation );
  SCDOM_RESULT SCFN( SciterGetElementNamespace)(  HELEMENT he, void* pval);
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
  SCDOM_RESULT SCFN( SciterNodeRemove)(HNODE hnode, SBOOL finalize);
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
  SBOOL SCFN( ValueIsNativeFunctor )( const VALUE* pval);
  // used to be script VM API
  LPVOID reserved1;
  LPVOID reserved2;
  LPVOID reserved3;
  LPVOID reserved4;

  HSARCHIVE SCFN( SciterOpenArchive ) (LPCBYTE archiveData, UINT archiveDataLength);
  SBOOL SCFN( SciterGetArchiveItem ) (HSARCHIVE harc, LPCWSTR path, LPCBYTE* pdata, UINT* pdataLength);
  SBOOL SCFN( SciterCloseArchive ) (HSARCHIVE harc);

  SCDOM_RESULT SCFN( SciterFireEvent)( const BEHAVIOR_EVENT_PARAMS* evt, SBOOL post, SBOOL *handled );

  LPVOID SCFN( SciterGetCallbackParam )(HWINDOW hwnd);
  UINT_PTR SCFN( SciterPostCallback )(HWINDOW hwnd, UINT_PTR wparam, UINT_PTR lparam, UINT timeoutms);

  LPSciterGraphicsAPI SCFN( GetSciterGraphicsAPI )();
  LPSciterRequestAPI SCFN( GetSciterRequestAPI )();

#if defined(WINDOWS)
  SBOOL SCFN( SciterCreateOnDirectXWindow ) (HWINDOW hwnd, IUnknown* pSwapChain); // IDXGISwapChain
  SBOOL SCFN( SciterRenderOnDirectXWindow ) (HWINDOW hwnd, HELEMENT elementToRenderOrNull, SBOOL frontLayer);
  SBOOL SCFN( SciterRenderOnDirectXTexture ) (HWINDOW hwnd, HELEMENT elementToRenderOrNull, IUnknown* surface); // IDXGISurface
#else 
  LPVOID SciterCreateOnDirectXWindow;
  LPVOID SciterRenderOnDirectXWindow;
  LPVOID SciterRenderOnDirectXTexture;
#endif

  SBOOL SCFN(SciterProcX)(HWINDOW hwnd, SCITER_X_MSG* pMsg ); // returns TRUE if handled

  UINT64 SCFN(SciterAtomValue)(const char* name); //
  SBOOL   SCFN(SciterAtomNameCB)(UINT64 atomv, LPCSTR_RECEIVER* rcv, LPVOID rcv_param);
  SBOOL   SCFN(SciterSetGlobalAsset)(som_asset_t* pass);

  SCDOM_RESULT SCFN(SciterGetElementAsset)(HELEMENT el, UINT64 nameAtom, som_asset_t** ppass);

  UINT   SCFN(SciterSetVariable)(HWINDOW hwndOrNull, LPCWSTR path, const VALUE* pvalToSet);
  UINT   SCFN(SciterGetVariable)(HWINDOW hwndOrNull, LPCWSTR path, VALUE* pvalToGet);

  UINT   SCFN(SciterElementUnwrap)(const VALUE* pval, HELEMENT* ppElement);
  UINT   SCFN(SciterElementWrap)(VALUE* pval, HELEMENT pElement);

  UINT   SCFN(SciterNodeUnwrap)(const VALUE* pval, HNODE* ppNode);
  UINT   SCFN(SciterNodeWrap)(VALUE* pval, HNODE pNode);


} ISciterAPI;

typedef ISciterAPI* (SCAPI *SciterAPI_ptr)();
#define HWINDOW_PTR HWINDOW*
#endif
