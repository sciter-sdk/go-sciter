#ifndef __SCITER_API_X__
#define __SCITER_API_X__

//|
//| Redirection of
//|

#include "sciter-x-types.h"
#include "sciter-x-dom.h"
#include "value.h"
// #include "tiscript.hpp"

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
  UINT SCFN( ValueEnumElements )( VALUE* pval, KeyValueCallback* penum, LPVOID param);
  UINT SCFN( ValueSetValueToKey )( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set);
  UINT SCFN( ValueGetValueOfKey )( const VALUE* pval, const VALUE* pkey, VALUE* pretval);
  UINT SCFN( ValueToString )( VALUE* pval, /*VALUE_STRING_CVT_TYPE*/ UINT how );
  UINT SCFN( ValueFromString )( VALUE* pval, LPCWSTR str, UINT strLength, /*VALUE_STRING_CVT_TYPE*/ UINT how );
  UINT SCFN( ValueInvoke )( VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url);
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

} ISciterAPI;

typedef ISciterAPI* (SCAPI *SciterAPI_ptr)();

#endif
