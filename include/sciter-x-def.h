/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

#ifndef __SCITER_X_DEF__
#define __SCITER_X_DEF__

#include "sciter-x-types.h"
#include "sciter-x-request.h"
#include "value.h"
#ifdef __cplusplus
  #include "aux-cvt.h"
  #include <iostream>
  #include <stdio.h>
  #include <stdarg.h>
  #include <wchar.h>
#endif


#define HAS_TISCRIPT

/** Resource data type.
 *  Used by SciterDataReadyAsync() function.
 **/

#define  HAS_TISCRIPT // in sciter

#include "sciter-x-value.h"
#include "sciter-x-dom.h"

/**Get name of Sciter window class.
 *
 * \return \b LPCWSTR, name of Sciter window class.
 *         \b NULL if initialization of the engine failed, Direct2D or DirectWrite are not supported on the OS.
 *
 * Use this function if you wish to create unicode version of Sciter.
 * The returned name can be used in CreateWindow(Ex)W function.
 * You can use #SciterClassNameT macro.
 **/
LPCWSTR SCAPI SciterClassName(void);

/**Returns major and minor version of Sciter engine.
  * \return UINT, hiword (16-bit) contains major number and loword contains minor number;
 **/
 UINT  SCAPI SciterVersion(SBOOL major);

/** #SC_LOAD_DATA notification return codes */
enum SC_LOAD_DATA_RETURN_CODES
{
  LOAD_OK = 0,      /**< do default loading if data not set */
  LOAD_DISCARD = 1, /**< discard request completely */
  LOAD_DELAYED = 2, /**< data will be delivered later by the host application.
                         Host application must call SciterDataReadyAsync(,,, requestId) on each LOAD_DELAYED request to avoid memory leaks. */
  LOAD_MYSELF  = 3, /**< you return LOAD_MYSELF result to indicate that your (the host) application took or will take care about HREQUEST in your code completely.
                         Use sciter-x-request.h[pp] API functions with SCN_LOAD_DATA::requestId handle . */
};

/**Notifies that Sciter is about to download a referred resource.
 *
 * \param lParam #LPSCN_LOAD_DATA.
 * \return #SC_LOAD_DATA_RETURN_CODES
 *
 * This notification gives application a chance to override built-in loader and
 * implement loading of resources in its own way (for example images can be loaded from
 * database or other resource). To do this set #SCN_LOAD_DATA::outData and
 * #SCN_LOAD_DATA::outDataSize members of SCN_LOAD_DATA. Sciter does not
 * store pointer to this data. You can call #SciterDataReady() function instead
 * of filling these fields. This allows you to free your outData buffer
 * immediately.
**/
#define SC_LOAD_DATA       0x01

/**This notification indicates that external data (for example image) download process
 * completed.
 *
 * \param lParam #LPSCN_DATA_LOADED
 *
 * This notifiaction is sent for each external resource used by document when
 * this resource has been completely downloaded. Sciter will send this
 * notification asynchronously.
 **/
#define SC_DATA_LOADED     0x02

/**This notification is sent when all external data (for example image) has been downloaded.
 *
 * This notification is sent when all external resources required by document
 * have been completely downloaded. Sciter will send this notification
 * asynchronously.
 **/
/* obsolete #define SC_DOCUMENT_COMPLETE 0x03
   use DOCUMENT_COMPLETE DOM event.
  */


/**This notification is sent on parsing the document and while processing
 * elements having non empty style.behavior attribute value.
 *
 * \param lParam #LPSCN_ATTACH_BEHAVIOR
 *
 * Application has to provide implementation of #sciter::behavior interface.
 * Set #SCN_ATTACH_BEHAVIOR::impl to address of this implementation.
 **/
#define SC_ATTACH_BEHAVIOR 0x04

/**This notification is sent when instance of the engine is destroyed.
 * It is always final notification.
 *
 * \param lParam #LPSCN_ENGINE_DESTROYED
 *
 **/
#define SC_ENGINE_DESTROYED 0x05

/**Posted notification.
 
 * \param lParam #LPSCN_POSTED_NOTIFICATION
 *
 **/
#define SC_POSTED_NOTIFICATION 0x06


/**This notification is sent when the engine encounters critical rendering error: e.g. DirectX gfx driver error.
   Most probably bad gfx drivers.
 
 * \param lParam #LPSCN_GRAPHICS_CRITICAL_FAILURE
 *
 **/
#define SC_GRAPHICS_CRITICAL_FAILURE 0x07


/**This notification is sent when the engine needs keyboard to be present on screen
   E.g. when <input|text> gets focus

 * \param lParam #LPSCN_KEYBOARD_REQUEST
 *
 **/
#define SC_KEYBOARD_REQUEST 0x08

/**This notification is sent when the engine needs some area to be redrawn
 
 * \param lParam #LPSCN_INVLIDATE_RECT
 *
 **/
#define SC_INVALIDATE_RECT 0x09


/**Notification callback structure.
 **/
typedef struct SCITER_CALLBACK_NOTIFICATION
{
  UINT code; /**< [in] one of the codes above.*/
  HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
} SCITER_CALLBACK_NOTIFICATION;

typedef SCITER_CALLBACK_NOTIFICATION * LPSCITER_CALLBACK_NOTIFICATION;

typedef UINT SC_CALLBACK SciterHostCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam );

typedef SciterHostCallback * LPSciterHostCallback;


/**This structure is used by #SC_LOAD_DATA notification.
 *\copydoc SC_LOAD_DATA
 **/

typedef struct SCN_LOAD_DATA
{
    UINT code;                 /**< [in] one of the codes above.*/
    HWINDOW hwnd;              /**< [in] HWINDOW of the window this callback was attached to.*/

    LPCWSTR  uri;              /**< [in] Zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/

    LPCBYTE  outData;          /**< [in,out] pointer to loaded data to return. if data exists in the cache then this field contain pointer to it*/
    UINT     outDataSize;      /**< [in,out] loaded data size to return.*/
    UINT     dataType;         /**< [in] SciterResourceType */

    HREQUEST requestId;        /**< [in] request handle that can be used with sciter-x-request API */

    HELEMENT principal;
    HELEMENT initiator;
} SCN_LOAD_DATA;

typedef SCN_LOAD_DATA*  LPSCN_LOAD_DATA;

/**This structure is used by #SC_DATA_LOADED notification.
 *\copydoc SC_DATA_LOADED
 **/
typedef struct SCN_DATA_LOADED
{
    UINT code;                 /**< [in] one of the codes above.*/
    HWINDOW hwnd;              /**< [in] HWINDOW of the window this callback was attached to.*/

    LPCWSTR  uri;              /**< [in] zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/
    LPCBYTE  data;             /**< [in] pointer to loaded data.*/
    UINT     dataSize;         /**< [in] loaded data size (in bytes).*/
    UINT     dataType;         /**< [in] SciterResourceType */
    UINT     status;           /**< [in]
                                         status = 0 (dataSize == 0) - unknown error.
                                         status = 100..505 - http response status, Note: 200 - OK!
                                         status > 12000 - wininet error code, see ERROR_INTERNET_*** in wininet.h
                                 */
} SCN_DATA_LOADED;

typedef SCN_DATA_LOADED * LPSCN_DATA_LOADED;

/**This structure is used by #SC_ATTACH_BEHAVIOR notification.
 *\copydoc SC_ATTACH_BEHAVIOR **/
typedef struct SCN_ATTACH_BEHAVIOR
{
    UINT code;                        /**< [in] one of the codes above.*/
    HWINDOW hwnd;                     /**< [in] HWINDOW of the window this callback was attached to.*/

    HELEMENT element;                 /**< [in] target DOM element handle*/
    LPCSTR   behaviorName;            /**< [in] zero terminated string, string appears as value of CSS behavior:"???" attribute.*/

    ElementEventProc* elementProc;    /**< [out] pointer to ElementEventProc function.*/
    LPVOID            elementTag;     /**< [out] tag value, passed as is into pointer ElementEventProc function.*/

} SCN_ATTACH_BEHAVIOR;
typedef SCN_ATTACH_BEHAVIOR* LPSCN_ATTACH_BEHAVIOR;

/**This structure is used by #SC_ENGINE_DESTROYED notification.
 *\copydoc SC_ENGINE_DESTROYED **/
typedef struct SCN_ENGINE_DESTROYED
{
    UINT code; /**< [in] one of the codes above.*/
    HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
} SCN_ENGINE_DESTROYED;

typedef SCN_ENGINE_DESTROYED* LPSCN_ENGINE_DESTROYED;

/**This structure is used by #SC_ENGINE_DESTROYED notification.
 *\copydoc SC_ENGINE_DESTROYED **/
typedef struct SCN_POSTED_NOTIFICATION
{
    UINT      code; /**< [in] one of the codes above.*/
    HWINDOW   hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
    UINT_PTR  wparam;
    UINT_PTR  lparam;
    UINT_PTR  lreturn;
} SCN_POSTED_NOTIFICATION;

typedef SCN_POSTED_NOTIFICATION* LPSCN_POSTED_NOTIFICATION;

/**This structure is used by #SC_GRAPHICS_CRITICAL_FAILURE notification.
 *\copydoc SC_GRAPHICS_CRITICAL_FAILURE **/
typedef struct SCN_GRAPHICS_CRITICAL_FAILURE
{
    UINT      code; /**< [in] = SC_GRAPHICS_CRITICAL_FAILURE */
    HWINDOW   hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
} SCN_GRAPHICS_CRITICAL_FAILURE;

typedef SCN_GRAPHICS_CRITICAL_FAILURE* LPSCN_GRAPHICS_CRITICAL_FAILURE;

/**This structure is used by #SC_KEYBOARD_REQUEST notification.
 *\copydoc SC_KEYBOARD_REQUEST **/
typedef struct SCN_KEYBOARD_REQUEST {
  UINT    code; /**< [in] = SC_KEYBOARD_REQUEST */
  HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
  UINT    keyboardMode; /**< [in] 0 - hide keyboard, 1 ... type of keyboard, TBD */
} SCN_KEYBOARD_REQUEST;

typedef SCN_KEYBOARD_REQUEST *LPSCN_KEYBOARD_REQUEST;

/**This structure is used by #SC_INVALIDATE_RECT notification.
 *\copydoc SC_INVALIDATE_RECT **/
typedef struct SCN_INVALIDATE_RECT {
  UINT    code; /**< [in] = SC_INVALIDATE_RECT */
  HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
  RECT    invalidRect; /**< [in] cumulative invalid rect.*/
} SCN_INVALIDATE_RECT;

typedef SCN_INVALIDATE_RECT *LPSCN_INVALIDATE_RECT;


#include "sciter-x-behavior.h"

/**This function is used in response to SCN_LOAD_DATA request.
 *
 * \param[in] hwnd \b HWINDOW, Sciter window handle.
 * \param[in] uri \b LPCWSTR, URI of the data requested by Sciter.
 * \param[in] data \b LPBYTE, pointer to data buffer.
 * \param[in] dataLength \b UINT, length of the data in bytes.
 * \return \b SBOOL, TRUE if Sciter accepts the data or \c FALSE if error occured
 * (for example this function was called outside of #SCN_LOAD_DATA request).
 *
 * \warning If used, call of this function MUST be done ONLY while handling
 * SCN_LOAD_DATA request and in the same thread. For asynchronous resource loading
 * use SciterDataReadyAsync
 **/
 SBOOL SCAPI SciterDataReady(HWINDOW hwnd,LPCWSTR uri,LPCBYTE data, UINT dataLength);

/**Use this function outside of SCN_LOAD_DATA request. This function is needed when you
 * you have your own http client implemented in your application.
 *
 * \param[in] hwnd \b HWINDOW, Sciter window handle.
 * \param[in] uri \b LPCWSTR, URI of the data requested by Sciter.
 * \param[in] data \b LPBYTE, pointer to data buffer.
 * \param[in] dataLength \b UINT, length of the data in bytes.
 * \param[in] requestId \b LPVOID, SCN_LOAD_DATA requestId, can be NULL.
 * \return \b SBOOL, TRUE if Sciter accepts the data or \c FALSE if error occured
 **/

 SBOOL SCAPI SciterDataReadyAsync(HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength,
                                         LPVOID requestId);

#ifdef WINDOWS

/**Sciter Window Proc.*/
 LRESULT SCAPI SciterProc(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**Sciter Window Proc without call of DefWindowProc.*/
 LRESULT SCAPI SciterProcND(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, SBOOL* pbHandled);

#endif


/**Load HTML file.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] url \b LPCWSTR, either absolute URL of HTML file to load. "file://...", "http://...", "res:...", "this://app/..." or absolute file path.
 * \return \b SBOOL, \c TRUE if the text was parsed and loaded successfully, \c FALSE otherwise.
 **/
 SBOOL SCAPI     SciterLoadFile(HWINDOW hWndSciter, LPCWSTR url);

/**Load HTML from in memory buffer with base.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] html \b LPCBYTE, Address of HTML to load.
 * \param[in] htmlSize \b UINT, Length of the array pointed by html parameter.
 * \param[in] baseUrl \b LPCWSTR, base URL. All relative links will be resolved against
 *                                this URL.
 * \return \b SBOOL, \c TRUE if the text was parsed and loaded successfully, FALSE otherwise.
 **/
 SBOOL SCAPI     SciterLoadHtml(HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl);

/**Set \link #SCITER_NOTIFY() notification callback function \endlink.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] cb \b SCITER_NOTIFY*, \link #SCITER_NOTIFY() callback function \endlink.
 * \param[in] cbParam \b LPVOID, parameter that will be passed to \link #SCITER_NOTIFY() callback function \endlink as vParam paramter.
 **/
 VOID SCAPI     SciterSetCallback(HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam);

/**Set Master style sheet.
 *
 * \param[in] utf8 \b LPCBYTE, start of CSS buffer.
 * \param[in] numBytes \b UINT, number of bytes in utf8.
 **/

 SBOOL SCAPI     SciterSetMasterCSS(LPCBYTE utf8, UINT numBytes);

/**Append Master style sheet.
 *
 * \param[in] utf8 \b LPCBYTE, start of CSS buffer.
 * \param[in] numBytes \b UINT, number of bytes in utf8.
 *
 **/

 SBOOL SCAPI     SciterAppendMasterCSS(LPCBYTE utf8, UINT numBytes);

/**Set (reset) style sheet of current document.
 Will reset styles for all elements according to given CSS (utf8)
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] utf8 \b LPCBYTE, start of CSS buffer.
 * \param[in] numBytes \b UINT, number of bytes in utf8.
 **/

 SBOOL SCAPI     SciterSetCSS(HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType);

/**Set media type of this sciter instance.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] mediaType \b LPCWSTR, media type name.
 *
 * For example media type can be "handheld", "projection", "screen", "screen-hires", etc.
 * By default sciter window has "screen" media type.
 *
 * Media type name is used while loading and parsing style sheets in the engine so
 * you should call this function *before* loading document in it.
 *
 **/

 SBOOL SCAPI     SciterSetMediaType(HWINDOW hWndSciter, LPCWSTR mediaType);

/**Set media variables of this sciter instance.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] mediaVars \b SCITER_VALUE, map that contains name/value pairs - media variables to be set.
 *
 * For example media type can be "handheld:true", "projection:true", "screen:true", etc.
 * By default sciter window has "screen:true" and "desktop:true"/"handheld:true" media variables.
 *
 * Media variables can be changed in runtime. This will cause styles of the document to be reset.
 *
 **/

 SBOOL SCAPI     SciterSetMediaVars(HWINDOW hWndSciter, const SCITER_VALUE *mediaVars);

 UINT SCAPI     SciterGetMinWidth(HWINDOW hWndSciter);
 UINT SCAPI     SciterGetMinHeight(HWINDOW hWndSciter, UINT width);

 SBOOL SCAPI     SciterCall(HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval);
// evalue script in context of current document
 SBOOL SCAPI     SciterEval( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval);

/**Update pending changes in Sciter window.
 *
 * \param[in] hwnd \b HWINDOW, Sciter window handle.
 *
 **/
 VOID SCAPI     SciterUpdateWindow(HWINDOW hwnd);

/** Try to translate message that sciter window is interested in.
 *
 * \param[in,out] lpMsg \b MSG*, address of message structure that was passed before to ::DispatchMessage(), ::PeekMessage().
 *
 * SciterTranslateMessage has the same meaning as ::TranslateMessage() and should be called immediately before it.
 * Example:
 *
 *   if( !SciterTranslateMessage(&msg) )
 *      TranslateMessage(&msg);
 *
 * ATTENTION!: SciterTranslateMessage call is critical for popup elements in MoSciter.
 *             On Desktop versions of the Sciter this function does nothing so can be ommited.
 *
 **/

#ifdef WINDOWS
 SBOOL SCAPI SciterTranslateMessage(MSG* lpMsg);
#endif

/**Set various options.
 *
 * \param[in] hWnd \b HWINDOW, Sciter window handle.
 * \param[in] option \b UINT, id of the option, one of SCITER_RT_OPTIONS
 * \param[in] option \b UINT, value of the option.
 *
 **/

typedef enum SCRIPT_RUNTIME_FEATURES
{
  ALLOW_FILE_IO = 0x00000001,
  ALLOW_SOCKET_IO = 0x00000002,
  ALLOW_EVAL = 0x00000004,
  ALLOW_SYSINFO = 0x00000008
} SCRIPT_RUNTIME_FEATURES;

typedef enum SCITER_RT_OPTIONS
{
   SCITER_SMOOTH_SCROLL = 1,      // value:TRUE - enable, value:FALSE - disable, enabled by default
   SCITER_CONNECTION_TIMEOUT = 2, // value: milliseconds, connection timeout of http client
   SCITER_HTTPS_ERROR = 3,        // value: 0 - drop connection, 1 - use builtin dialog, 2 - accept connection silently
   SCITER_FONT_SMOOTHING = 4,     // value: 0 - system default, 1 - no smoothing, 2 - std smoothing, 3 - clear type

   SCITER_TRANSPARENT_WINDOW = 6, // Windows Aero support, value:
                                  // 0 - normal drawing,
                                  // 1 - window has transparent background after calls DwmExtendFrameIntoClientArea() or DwmEnableBlurBehindWindow().
   SCITER_SET_GPU_BLACKLIST  = 7, // hWnd = NULL,
                                  // value = LPCBYTE, json - GPU black list, see: gpu-blacklist.json resource.
   SCITER_SET_SCRIPT_RUNTIME_FEATURES = 8, // value - combination of SCRIPT_RUNTIME_FEATURES flags.
   SCITER_SET_GFX_LAYER = 9,      // hWnd = NULL, value - GFX_LAYER
   SCITER_SET_DEBUG_MODE = 10,    // hWnd, value - TRUE/FALSE
   SCITER_SET_UX_THEMING = 11,    // hWnd = NULL, value - SBOOL, TRUE - the engine will use "unisex" theme that is common for all platforms. 
                                  // That UX theme is not using OS primitives for rendering input elements. Use it if you want exactly
                                  // the same (modulo fonts) look-n-feel on all platforms.

   SCITER_ALPHA_WINDOW  = 12,     //  hWnd, value - TRUE/FALSE - window uses per pixel alpha (e.g. WS_EX_LAYERED/UpdateLayeredWindow() window)
   
   SCITER_SET_INIT_SCRIPT = 13,   // hWnd - N/A , value LPCSTR - UTF-8 encoded script source to be loaded into each view before any other script execution.
                                  //                             The engine copies this string inside the call.

   SCITER_SET_MAIN_WINDOW = 14,   //  hWnd, value - TRUE/FALSE - window is main, will destroy all other dependent windows on close

   SCITER_SET_MAX_HTTP_DATA_LENGTH = 15, // hWnd - N/A , value - max request length in megabytes (1024*1024 bytes)

   SCITER_SET_PX_AS_DIP = 16, // value 1 - 1px in CSS is treated as 1dip, value 0 - default behavior - 1px is a physical pixel 

   SCITER_ENABLE_UIAUTOMATION = 17,  // hWnd - N/A , TRUE/FALSE, enables UIAutomation support. 

} SCITER_RT_OPTIONS;

 SBOOL SCAPI SciterSetOption(HWINDOW hWnd, UINT option, UINT_PTR value );

/**Get current pixels-per-inch metrics of the Sciter window
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[out] px \b PUINT, get ppi in horizontal direction.
 * \param[out] py \b PUINT, get ppi in vertical direction.
 *
 **/

 VOID SCAPI SciterGetPPI(HWINDOW hWndSciter, UINT* px, UINT* py);

/**Get "expando" of the view object
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[out] pval \b VALUE*, expando as sciter::value.
 *
 **/

 SBOOL SCAPI SciterGetViewExpando( HWINDOW hwnd, VALUE* pval );

typedef struct URL_DATA
{
  LPCSTR             requestedUrl;   // requested URL
  LPCSTR             realUrl;        // real URL data arrived from (after possible redirections)
  SciterResourceType requestedType;  // requested data category: html, script, image, etc.
  LPCSTR             httpHeaders;    // if any
  LPCSTR             mimeType;       // mime type reported by server (if any)
  LPCSTR             encoding;       // data encoding (if any)
  LPCBYTE            data;
  UINT              dataLength;
} URL_DATA;

typedef VOID SC_CALLBACK URL_DATA_RECEIVER( const URL_DATA* pUrlData, LPVOID param );


/** Get url resource data received by the engine
 *  Note: this function really works only if the engine is set to debug mode.
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] receiver \b URL_DATA_RECEIVER, address of reciver callback.
 * \param[in] param \b LPVOID, param passed to callback as it is.
 * \param[in] url \b LPCSTR, optional, url of the data. If not provided the engine will list all loaded resources
 * \return \b SBOOL, \c TRUE if receiver is called at least once, FALSE otherwise.
 *
 **/

 SBOOL SCAPI SciterEnumUrlData(HWINDOW hWndSciter, URL_DATA_RECEIVER* receiver, LPVOID param, LPCSTR url);


#ifdef WINDOWS

/**Creates instance of Sciter Engine on window controlled by DirectX
*
* \param[in] hwnd \b HWINDOW, window handle to create Sciter on.
* \param[in] IDXGISwapChain \b pSwapChain,  reference of IDXGISwapChain created on the window.
* \return \b SBOOL, \c TRUE if engine instance is created, FALSE otherwise.
*
**/

SBOOL SCAPI SciterCreateOnDirectXWindow(HWINDOW hwnd, IUnknown* pSwapChain); // IDXGISwapChain

/**Renders content of the document loaded into the window
* Optionally allows to render parts of document (separate DOM elements) as layers
*
* \param[in] hwnd \b HWINDOW, window handle to create Sciter on.
* \param[in] HELEMENT \b elementToRenderOrNull,  html element to render. If NULL then the engine renders whole document.
* \param[in] SBOOL \b frontLayer,  TRUE if elementToRenderOrNull is not NULL and this is the topmost layer.
* \return \b SBOOL, \c TRUE if layer was rendered successfully.
*
**/
SBOOL SCAPI SciterRenderOnDirectXWindow(HWINDOW hwnd, HELEMENT elementToRenderOrNull /* = NULL*/, SBOOL frontLayer /* = FALSE*/);

/**Renders content of the document loaded to DXGI texture
* Optionally allows to render parts of document (separate DOM elements) as layers
*
* \param[in] HWINDOW \b hwnd, window handle to create Sciter on.
* \param[in] HELEMENT \b elementToRenderOrNull,  html element to render. If NULL then the engine renders whole document.
* \param[in] IDXGISurface \b surface, DirectX 2D texture to render in.
* \return \b SBOOL, \c TRUE if layer was rendered successfully.
*
**/
SBOOL SCAPI SciterRenderOnDirectXTexture(HWINDOW hwnd, HELEMENT elementToRenderOrNull, IUnknown* surface); // IDXGISurface


/**Render document to ID2D1RenderTarget
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] ID2D1RenderTarget \b prt, Direct2D render target.
 * \return \b SBOOL, \c TRUE if hBmp is 24bpp or 32bpp, FALSE otherwise.
 *
 **/

 SBOOL SCAPI SciterRenderD2D(HWINDOW hWndSciter, IUnknown* /*ID2D1RenderTarget**/ prt);

/** Obtain pointer to ID2D1Factory instance used by the engine:
 *
 * \param[in] ID2D1Factory \b **ppf, address of variable receiving pointer of ID2D1Factory.
 * \return \b SBOOL, \c TRUE if parameters are valid and *ppf was set by valid pointer.
 *
 * NOTE 1: ID2D1Factory returned by the function is "add-refed" - it is your responsibility to call Release() on it.
 * NOTE 2: *ppf variable shall be initialized to NULL before calling the function.
 *
 **/

 SBOOL SCAPI     SciterD2DFactory(IUnknown** /*ID2D1Factory ***/ ppf);

/** Obtain pointer to IDWriteFactory instance used by the engine:
 *
 * \param[in] IDWriteFactory \b **ppf, address of variable receiving pointer of IDWriteFactory.
 * \return \b SBOOL, \c TRUE if parameters are valid and *ppf was set by valid pointer.
 *
 * NOTE 1: IDWriteFactory returned by the function is "add-refed" - it is your responsibility to call Release() on it.
 * NOTE 2: *ppf variable shall be initialized to NULL before calling the function.
 *
 **/

 SBOOL SCAPI     SciterDWFactory(IUnknown** /*IDWriteFactory ***/ ppf);

#endif

/** Get graphics capabilities of the system
 *
 * \pcaps[in] LPUINT \b pcaps, address of variable receiving:
 *                             0 - no compatible graphics found;
 *                             1 - compatible graphics found but Direct2D will use WARP driver (software emulation);
 *                             2 - Direct2D will use hardware backend (best performance);
 * \return \b SBOOL, \c TRUE if pcaps is valid pointer.
 *
 **/

 SBOOL SCAPI     SciterGraphicsCaps(LPUINT pcaps);


/** Set sciter home url.
 *  home url is used for resolving sciter: urls
 *  If you will set it like SciterSetHomeURL(hwnd,"http://sciter.com/modules/")
 *  then <script src="sciter:lib/root-extender.tis"> will load
 *  root-extender.tis from http://sciter.com/modules/lib/root-extender.tis
 *
 * \param[in] hWndSciter \b HWINDOW, Sciter window handle.
 * \param[in] baseUrl \b LPCWSTR, URL of sciter home.
 *
 **/

 SBOOL SCAPI     SciterSetHomeURL(HWINDOW hWndSciter, LPCWSTR baseUrl);

#if defined(OSX)
   HWINDOW SCAPI  SciterCreateNSView( LPRECT frame ); // returns NSView*
  typedef LPVOID SciterWindowDelegate; // Obj-C id, NSWindowDelegate and NSResponder
#elif defined(WINDOWS)
  typedef LRESULT SC_CALLBACK SciterWindowDelegate(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID pParam, SBOOL* handled);
#elif defined(LINUX)
  typedef LPVOID SciterWindowDelegate;
#endif

typedef enum SCITER_CREATE_WINDOW_FLAGS {
   SW_CHILD      = (1 << 0), // child window only, if this flag is set all other flags ignored
   SW_TITLEBAR   = (1 << 1), // toplevel window, has titlebar
   SW_RESIZEABLE = (1 << 2), // has resizeable frame
   SW_TOOL       = (1 << 3), // is tool window
   SW_CONTROLS   = (1 << 4), // has minimize / maximize buttons
   SW_GLASSY     = (1 << 5), // glassy window - supports "Acrylic" on Windows and "Vibrant" on MacOS. 
   SW_ALPHA      = (1 << 6), // transparent window ( e.g. WS_EX_LAYERED on Windows )
   SW_MAIN       = (1 << 7), // main window of the app, will terminate the app on close
   SW_POPUP      = (1 << 8), // the window is created as topmost window.
   SW_ENABLE_DEBUG = (1 << 9), // make this window inspector ready
   SW_OWNS_VM      = (1 << 10), // it has its own script VM
} SCITER_CREATE_WINDOW_FLAGS;

#if !defined(WINDOWLESS)
/** Create sciter window.
 *  On Windows returns HWND of either top-level or child window created.
 *  On OS X returns NSView* of either top-level window or child view .
 *
 * \param[in] creationFlags \b SCITER_CREATE_WINDOW_FLAGS, creation flags.
 * \param[in] frame \b LPRECT, window frame position and size.
 * \param[in] delegate \b SciterWindowDelegate, either partial WinProc implementation or thing implementing NSWindowDelegate protocol.
 * \param[in] delegateParam \b LPVOID, optional param passed to SciterWindowDelegate.
 * \param[in] parent \b HWINDOW, optional parent window.
 *
 **/
 HWINDOW SCAPI  SciterCreateWindow( UINT creationFlags,
                                            LPRECT frame,
                                            SciterWindowDelegate* delegate,
                                            LPVOID delegateParam,
                                            HWINDOW parent);

#endif

/** SciterSetupDebugOutput - setup debug output function.
 *
 *  This output function will be used for reprting problems
 *  found while loading html and css documents.
 *
 **/

typedef enum OUTPUT_SUBSYTEMS
{
   OT_DOM = 0,       // html parser & runtime
   OT_CSSS,          // csss! parser & runtime
   OT_CSS,           // css parser
   OT_TIS,           // TIS parser & runtime
} OUTPUT_SUBSYTEMS;

typedef enum OUTPUT_SEVERITY
{
  OS_INFO,
  OS_WARNING,
  OS_ERROR,
} OUTPUT_SEVERITY;

typedef VOID (SC_CALLBACK* DEBUG_OUTPUT_PROC)(LPVOID param, UINT subsystem /*OUTPUT_SUBSYTEMS*/, UINT severity, LPCWSTR text, UINT text_length);

 VOID SCAPI SciterSetupDebugOutput(
                HWINDOW                  hwndOrNull,// HWINDOW or null if this is global output handler
                LPVOID                param,     // param to be passed "as is" to the pfOutput
                DEBUG_OUTPUT_PROC     pfOutput   // output function, output stream alike thing.
                );

#endif

