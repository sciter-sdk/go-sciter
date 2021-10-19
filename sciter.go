// This package provides a Golang bindings of Sciter using cgo.
// Using go sciter you must have the platform specified sciter dynamic library
// downloaded from sciter-sdk,
// the library itself is rather small (under 5MB, less than 2MB when upxed) .
package sciter

/*
#cgo CFLAGS: -g -std=c11 -Iinclude -DPLAIN_API_ONLY -D_BSD_SOURCE -D_DEFAULT_SOURCE
#cgo linux LDFLAGS: -ldl
#cgo linux pkg-config: gtk+-3.0

#include "sciter-x.h"

extern SBOOL SC_CALLBACK SciterElementCallback_cgo(HELEMENT he, LPVOID param);
extern VOID SC_CALLBACK LPCSTR_RECEIVER_cgo( LPCSTR str, UINT str_length, LPVOID param );
extern VOID SC_CALLBACK LPCWSTR_RECEIVER_cgo( LPCWSTR str, UINT str_length, LPVOID param );
extern VOID SC_CALLBACK LPCBYTE_RECEIVER_cgo( LPCBYTE bytes, UINT num_bytes, LPVOID param );
extern SBOOL SC_CALLBACK ElementEventProc_cgo(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms );
extern UINT SC_CALLBACK SciterHostCallback_cgo( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam );
// native functor
extern VOID NATIVE_FUNCTOR_INVOKE_cgo( VOID* tag, UINT argc, const VALUE* argv, VALUE* retval);
extern VOID NATIVE_FUNCTOR_RELEASE_cgo( VOID* tag );
// cmp
extern INT SC_CALLBACK ELEMENT_COMPARATOR_cgo( HELEMENT he1, HELEMENT he2, LPVOID param );
// ValueEnumElements
extern SBOOL SC_CALLBACK KeyValueCallback_cgo(LPVOID param, const VALUE* pkey, const VALUE* pval );

extern const char * SCITER_DLL_PATH;

extern HSARCHIVE SCAPI SciterOpenArchive (LPCBYTE archiveData, UINT archiveDataLength);

extern SBOOL SCAPI SciterGetArchiveItem (HSARCHIVE harc, LPCWSTR path, LPCBYTE* pdata, UINT* pdataLength);

extern SBOOL SCAPI SciterCloseArchive (HSARCHIVE harc);

*/
import "C"
import (
	"fmt"
	"log"
	"runtime"
	"strings"
	"unsafe"
)

type Sciter struct {
	hwnd C.HWINDOW
	// because it live with the PROGRAM
	callbacks map[*CallbackHandler]struct{}
	// map scripting function name to NativeFunctor
	*eventMapper
	// sciter archive
	har C.HSARCHIVE
}

var (
	BAD_HWINDOW C.HWINDOW = nil
)

func Wrap(hwnd C.HWINDOW) *Sciter {
	s := &Sciter{
		hwnd:      hwnd,
		callbacks: make(map[*CallbackHandler]struct{}),
	}
	return s
}

func ClassName() string {
	cname := (*uint16)(unsafe.Pointer(C.SciterClassName()))
	return Utf16ToString(cname)
}

func Version(major bool) uint {
	var v C.UINT
	if major {
		v = C.SciterVersion(C.SBOOL(1))
	} else {
		v = C.SciterVersion(C.SBOOL(0))
	}
	return uint(v)
}

func VersionAsString() string {
	major := Version(true)
	minor := Version(false)
	return fmt.Sprintf("0x%x %x", major, minor)
}

func (s *Sciter) GetHwnd() C.HWINDOW {
	return s.hwnd
}

func SetDLL(dir string) {
	C.SCITER_DLL_PATH = C.CString(dir)
}

//This function is used in response to SCN_LOAD_DATA request.
//
//  \param[in] hwnd \b HWINDOW, Sciter window handle.
//  \param[in] uri \b LPCWSTR, URI of the data requested by Sciter.
//  \param[in] data \b LPBYTE, pointer to data buffer.
//  \param[in] dataLength \b UINT, length of the data in bytes.
//  \return \b BOOL, TRUE if Sciter accepts the data or \c FALSE if error occured
//  (for example this function was called outside of #SCN_LOAD_DATA request).
//
//  \warning If used, call of this function MUST be done ONLY while handling
//  SCN_LOAD_DATA request and in the same thread. For asynchronous resource loading
//  use SciterDataReadyAsync
func (s *Sciter) DataReady(uri string, data []byte) bool {
	var pData C.LPCBYTE
	if len(data) > 0 {
		pData = (C.LPCBYTE)(unsafe.Pointer(&data[0]))
	}
	ret := C.SciterDataReady(s.hwnd, StringToWcharPtr(uri), pData, C.UINT(len(data)))
	if ret == 0 {
		return false
	}
	// mark the data to prevent gc
	loadedUri[uri] = data
	return true
}

var (
	loadedUri = map[string][]byte{}
)

//SciterDataReadyAsync (HWINDOW hwnd,LPCWSTR uri, LPCBYTE data, UINT dataLength, LPVOID requestId) ;//{ return SAPI()->SciterDataReadyAsync (hwnd,uri, data, dataLength, requestId); }

//Use this function outside of SCN_LOAD_DATA request. This function is needed when you
//  you have your own http client implemented in your application.
//
//  \param[in] hwnd \b HWINDOW, Sciter window handle.
//  \param[in] uri \b LPCWSTR, URI of the data requested by Sciter.
//  \param[in] data \b LPBYTE, pointer to data buffer.
//  \param[in] requestId \b LPVOID, SCN_LOAD_DATA requestId.
//  \return \b BOOL, TRUE if Sciter accepts the data or \c FALSE if error occured
func (s *Sciter) DataReadyAsync(uri string, data []byte, requestId C.HREQUEST) bool {
	// args
	var pData C.LPCBYTE
	if len(data) > 0 {
		pData = (C.LPCBYTE)(unsafe.Pointer(&data[0]))
	}
	curi := StringToWcharPtr(uri)
	cdataLength := C.UINT(len(data))
	crequestId := C.LPVOID(requestId)

	// cgo call
	ret := C.SciterDataReadyAsync(s.hwnd, curi, pData, cdataLength, crequestId)
	if ret == 0 {
		return false
	}
	return true
}

// BOOL SciterLoadFile (HWINDOW hWndSciter, LPCWSTR filename) ;//{ return SAPI()->SciterLoadFile (hWndSciter,filename); }

func (s *Sciter) LoadFile(filename string) error {
	ret := C.SciterLoadFile(s.hwnd, StringToWcharPtr(filename))
	if ret == 0 {
		return fmt.Errorf("LoadFile with: %s failed", filename)
	}
	return nil
}

// BOOL SciterLoadHtml (HWINDOW hWndSciter, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl) ;//{ return SAPI()->SciterLoadHtml (hWndSciter,html,htmlSize,baseUrl); }

//Load HTML from in memory buffer with base.
//
//  \param[in] hWndSciter \b HWINDOW, Sciter window handle.
//  \param[in] html \b LPCBYTE, Address of HTML to load.
//  \param[in] htmlSize \b UINT, Length of the array pointed by html parameter.
//  \param[in] baseUrl \b LPCWSTR, base URL. All relative links will be resolved against
//                                 this URL.
//  \return \b BOOL, \c TRUE if the text was parsed and loaded successfully, FALSE otherwise.
func (s *Sciter) LoadHtml(html, baseUrl string) error {
	// args
	chtml := (C.LPCBYTE)(unsafe.Pointer(StringToBytePtr(html)))
	csize := C.UINT(len(html))
	cbaseUrl := StringToWcharPtr(baseUrl)
	// cgo call
	ret := C.SciterLoadHtml(s.hwnd, chtml, csize, cbaseUrl)
	if ret == 0 {
		return fmt.Errorf("LoadHtml with: %s failed", string(html))
	}
	return nil
}

// typedef UINT SC_CALLBACK SciterHostCallback( LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam );

//export goSciterHostCallback
func goSciterHostCallback(ph unsafe.Pointer, callbackParam unsafe.Pointer) int {
	phdr := (*SciterCallbackNotification)(ph)
	handler := globalCallbackHandlers[int(uintptr(callbackParam))]
	switch phdr.Code {
	case SC_LOAD_DATA:
		if handler.OnLoadData != nil {
			return handler.OnLoadData((*ScnLoadData)(unsafe.Pointer(phdr)))
		}
	case SC_DATA_LOADED:
		if handler.OnDataLoaded != nil {
			return handler.OnDataLoaded((*ScnDataLoaded)(unsafe.Pointer(phdr)))
		}
	case SC_ATTACH_BEHAVIOR:
		params := (*ScnAttachBehavior)(unsafe.Pointer(phdr))
		key := params.BehaviorName()
		if behavior, exists := handler.Behaviors[key]; exists {
			// Increment the reference count for this behavior
			if refCount, exists := behaviors[behavior]; exists {
				behaviors[behavior] = refCount + 1
			} else {
				behaviors[behavior] = 1
			}
			el := WrapElement(params.Element())
			el.attachBehavior(behavior)
		} else {
			log.Printf("No such behavior <%s> found", key)
		}
	case SC_ENGINE_DESTROYED:
		if handler.OnEngineDestroyed != nil {
			return handler.OnEngineDestroyed()
		}
	case SC_POSTED_NOTIFICATION:
		if handler.OnPostedNotification != nil {
			return handler.OnPostedNotification((*ScnPostedNotification)(unsafe.Pointer(phdr)))
		}
	case SC_GRAPHICS_CRITICAL_FAILURE:
		if handler.OnGraphicsCriticalFailure != nil {
			return handler.OnGraphicsCriticalFailure()
		}
	case SC_KEYBOARD_REQUEST:
		if handler.OnKeyboardRequest != nil {
			return handler.OnKeyboardRequest((*ScnKeyboardRequest)(unsafe.Pointer(phdr)))
		}
	case SC_INVALIDATE_RECT:
		if handler.OnInvalidateRect != nil {
			return handler.OnInvalidateRect((*ScnInvalidateRect)(unsafe.Pointer(phdr)))
		}
	}
	return 0
}

var (
	sciterhostcallback     = C.LPSciterHostCallback(unsafe.Pointer(C.SciterHostCallback_cgo))
	globalCallbackHandlers = make([]*CallbackHandler, 0)
)

// VOID SciterSetCallback (HWINDOW hWndSciter, LPSciterHostCallback cb, LPVOID cbParam)

//Set \link #SCITER_NOTIFY() notification callback function \endlink.
//
//  \param[in] hWndSciter \b HWINDOW, Sciter window handle.
//  \param[in] cb \b SCITER_NOTIFY*, \link #SCITER_NOTIFY() callback function \endlink.
//  \param[in] cbParam \b LPVOID, parameter that will be passed to \link #SCITER_NOTIFY() callback function \endlink as vParam paramter.
func (s *Sciter) SetCallback(handler *CallbackHandler) {
	if _, ok := s.callbacks[handler]; ok {
		return
	}
	s.callbacks[handler] = struct{}{}
	// args
	globalCallbackHandlers = append(globalCallbackHandlers, handler)
	idx := len(globalCallbackHandlers) - 1
	cparam := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	// cgo call
	C.SciterSetCallback(s.hwnd, sciterhostcallback, cparam)
}

// BOOL SciterSetMasterCSS (LPCBYTE utf8, UINT numBytes)

//Set Master style sheet.
//
//  \param[in] utf8 \b LPCBYTE, start of CSS buffer.
//  \param[in] numBytes \b UINT, number of bytes in utf8.
func SetMasterCSS(data string) (ok bool) {
	// args
	cutf8 := C.LPCBYTE(unsafe.Pointer(StringToBytePtr(data)))
	clen := C.UINT(len(data))
	// cgo call
	r := C.SciterSetMasterCSS(cutf8, clen)
	if r == 0 {
		return false
	}
	return true
}

// BOOL SciterAppendMasterCSS (LPCBYTE utf8, UINT numBytes)

//Append Master style sheet.
//
//  \param[in] utf8 \b LPCBYTE, start of CSS buffer.
//  \param[in] numBytes \b UINT, number of bytes in utf8.
func AppendMasterCSS(data string) (ok bool) {
	// args
	cutf8 := C.LPCBYTE(unsafe.Pointer(StringToBytePtr(data)))
	clen := C.UINT(len(data))
	// cgo call
	r := C.SciterAppendMasterCSS(cutf8, clen)
	if r == 0 {
		return false
	}
	return true
}

// BOOL SciterSetCSS (HWINDOW hWndSciter, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType)

//Set (reset) style sheet of current document.
//Will reset styles for all elements according to given CSS (utf8)
//
//  \param[in] hWndSciter \b HWINDOW, Sciter window handle.
//  \param[in] utf8 \b LPCBYTE, start of CSS buffer.
//  \param[in] numBytes \b UINT, number of bytes in utf8.
func (s *Sciter) SetCSS(css, baseUrl, mediaType string) (ok bool) {
	// args
	ccss := C.LPCBYTE(unsafe.Pointer(StringToBytePtr(css)))
	clen := C.UINT(len(css))
	cbaseUrl := StringToWcharPtr(baseUrl)
	cmediaType := StringToWcharPtr(mediaType)
	// cgo call
	r := C.SciterSetCSS(s.hwnd, ccss, clen, cbaseUrl, cmediaType)
	if r == 0 {
		return false
	}
	return true
}

// BOOL SciterSetMediaType (HWINDOW hWndSciter, LPCWSTR mediaType)

//Set media type of this sciter instance.
//
//  \param[in] hWndSciter \b HWINDOW, Sciter window handle.
//  \param[in] mediaType \b LPCWSTR, media type name.
//
//  For example media type can be "handheld", "projection", "screen", "screen-hires", etc.
//  By default sciter window has "screen" media type.
//
//  Media type name is used while loading and parsing style sheets in the engine so
//  you should call this function *before* loading document in it.
func (s *Sciter) SetMediaType(mediaType string) (ok bool) {
	// args
	cmediaType := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(mediaType)))
	// cgo call
	r := C.SciterSetMediaType(s.hwnd, cmediaType)
	if r == 0 {
		return false
	}
	return true
}

// BOOL SciterSetMediaVars (HWINDOW hWndSciter, const SCITER_VALUE *mediaVars)

// UINT SciterGetMinWidth (HWINDOW hWndSciter)
func (s *Sciter) MinWidth() int {
	r := C.SciterGetMinWidth(s.hwnd)
	return int(r)
}

// UINT SciterGetMinHeight (HWINDOW hWndSciter, UINT width)
func (s *Sciter) MinHeight(width int) int {
	// args
	cwidth := C.UINT(width)
	// cgo call
	r := C.SciterGetMinHeight(s.hwnd, cwidth)
	return int(r)
}

// BOOL SciterCall (HWINDOW hWnd, LPCSTR functionName, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE* retval)

// To call global function defined in script using its full name (may include name of namespaces where it resides)
func (s *Sciter) Call(functionName string, args ...*Value) (retval *Value, err error) {
	retval = NewValue()
	argc := len(args)
	argv := make([]Value, argc)
	for i := 0; i < argc; i++ {
		argv[i].Copy(args[i]) // Make copy that is not garbage collected
		defer argv[i].finalize()
	}
	// args
	funcName := C.CString(functionName)
	defer C.free(unsafe.Pointer(funcName))
	cfn := C.LPCSTR(unsafe.Pointer(funcName))
	cargc := C.UINT(argc)
	var cargv *C.SCITER_VALUE
	if len(argv) > 0 {
		cargv = (*C.SCITER_VALUE)(unsafe.Pointer(&argv[0]))
	}
	cretval := (*C.SCITER_VALUE)(unsafe.Pointer(retval))
	// cgo call
	r := C.SciterCall(s.hwnd, cfn, cargc, cargv, cretval)
	if r == 0 {
		err = fmt.Errorf("SciterCall failed with function: %s", functionName)
	}
	return
}

// BOOL SciterEval ( HWINDOW hwnd, LPCWSTR script, UINT scriptLength, SCITER_VALUE* pretval)

// To evaluate arbitrary script in context of current document loaded into the window
func (s *Sciter) Eval(script string) (retval *Value, ok bool) {
	retval = NewValue()
	// args
	u16, err := Utf16FromString(script)
	if err != nil {
		return nil, false
	}
	cscriptLength := C.UINT(len(u16) - 1)
	cscript := C.LPCWSTR(unsafe.Pointer(&u16[0]))
	cretval := (*C.SCITER_VALUE)(unsafe.Pointer(retval))
	// cgo call
	r := C.SciterEval(s.hwnd, cscript, cscriptLength, cretval)
	if r == 0 {
		ok = false
	} else {
		ok = true
	}
	return
}

// VOID SciterUpdateWindow(HWINDOW hwnd)
func (s *Sciter) UpdateWindow() {
	C.SciterUpdateWindow(s.hwnd)
}

// #ifdef WINDOWS
// BOOL     SciterTranslateMessage (MSG* lpMsg) ;//{ return SAPI()->SciterTranslateMessage (lpMsg); }
// #endif

// BOOL     SciterSetOption (HWINDOW hWnd, UINT option, UINT_PTR value )
func (s *Sciter) SetOption(option Sciter_RT_OPTIONS, value uint) (ok bool) {
	// args
	coption := C.UINT(option)
	cvalue := C.UINT_PTR(value)
	// cgo call
	r := C.SciterSetOption(s.hwnd, coption, cvalue)
	if r == 0 {
		return false
	}
	return true
}

// BOOL     SciterSetOption (NULL, UINT option, UINT_PTR value )
func SetOption(option Sciter_RT_OPTIONS, value uint) (ok bool) {
	coption := C.UINT(option)
	cvalue := C.UINT_PTR(value)
	hwnd := BAD_HWINDOW
	r := C.SciterSetOption(hwnd, coption, cvalue)
	if r == 0 {
		return false
	}
	return true
}

// VOID     SciterGetPPI (HWINDOW hWndSciter, UINT* px, UINT* py) ;// { SAPI()->SciterGetPPI (hWndSciter,px,py); }
// BOOL     SciterGetViewExpando ( HWINDOW hwnd, VALUE* pval ) ;//{ return SAPI()->SciterGetViewExpando ( hwnd, pval ); }
// #ifdef WINDOWS
// BOOL     SciterRenderD2D (HWINDOW hWndSciter, ID2D1RenderTarget* prt) ;//{ return SAPI()->SciterRenderD2D (hWndSciter,prt); }
// BOOL     SciterD2DFactory (ID2D1Factory ** ppf) ;//{ return SAPI()->SciterD2DFactory (ppf); }
// BOOL     SciterDWFactory (IDWriteFactory ** ppf) ;//{ return SAPI()->SciterDWFactory (ppf); }
// #endif
// BOOL     SciterGraphicsCaps (LPUINT pcaps) ;//{ return SAPI()->SciterGraphicsCaps (pcaps); }

// BOOL     SciterSetHomeURL (HWINDOW hWndSciter, LPCWSTR baseUrl)

// Set sciter home url.
//   home url is used for resolving sciter: urls
//   If you will set it like SciterSetHomeURL(hwnd,"http://sciter.com/modules/")
//   then <script src="sciter:lib/root-extender.tis"> will load
//   root-extender.tis from http://sciter.com/modules/lib/root-extender.tis
//
//  \param[in] hWndSciter \b HWINDOW, Sciter window handle.
//  \param[in] baseUrl \b LPCWSTR, URL of sciter home.
func (s *Sciter) SetHomeURL(baseUrl string) (ok bool) {
	// args
	cbaseUrl := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(baseUrl)))
	// cgo call
	r := C.SciterSetHomeURL(s.hwnd, cbaseUrl)
	if r == 0 {
		return false
	}
	return true
}

// Open data blob of the provided compressed Sciter archive.
func (s *Sciter) OpenArchive(data []byte) {
	s.har = C.SciterOpenArchive((*C.BYTE)(&data[0]), C.UINT(len(data)))
}

// Get an archive item referenced by \c uri.
//
// Usually it is passed to \c Sciter.DataReady().
func (s *Sciter) GetArchiveItem(uri string) []byte {
	var data C.LPCBYTE
	var length C.UINT
	cdata := (*C.LPCBYTE)(&data)
	r := C.SciterGetArchiveItem(s.har, StringToWcharPtr(uri), cdata, &length)
	if r == 0 {
		return nil
	}
	ret := ByteCPtrToBytes(data, length)
	return ret
}

// Close the archive.
func (s *Sciter) CloseArchive() {
	C.SciterCloseArchive(s.har)
	s.har = C.HSARCHIVE(nil)
}

// Register `this://app/` URLs to be loaded from the given Sciter archive.
//
// Pack resources using `packfolder` tool:
//
//   `$ packfolder res_folder res_packed.go -v resource_name -go`
//
// Usage:
//
//```
//   win.SetResourceArchive(resource_name)
//   win.LoadFile("this://app//index.htm")
//```
func (s *Sciter) SetResourceArchive(data []byte) {

	// register `this://app/` schema
	callback := &CallbackHandler{
		OnLoadData: func(params *ScnLoadData) int {
			if strings.HasPrefix(params.Uri(), "this://app/") {
				// load resource starting with our schema
				url := params.Uri()[11:]
				fileData := s.GetArchiveItem(url)
				if fileData != nil {
					// use loaded resource
					s.DataReady(url, fileData)
				} else {
					// failed to load
					log.Println("error: failed to load " + params.Uri())
					//  but fallback to Sciter anyway
				}
			}
			return LOAD_OK
		},
	}

	s.OpenArchive(data)
	s.SetCallback(callback)
}

// #if defined(OSX)
// HWINDOW  SciterCreateNSView ( LPRECT frame ) ;//{ return SAPI()->SciterCreateNSView ( frame ); }
// #endif
// HWINDOW  SciterCreateWindow ( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent);

// #if defined(OSX)
//    HWINDOW SCAPI  SciterCreateNSView( LPRECT frame ); // returns NSView*
//   typedef LPVOID SciterWindowDelegate; // Obj-C id, NSWindowDelegate and NSResponder
// #elif defined(WINDOWS)
//   typedef LRESULT SC_CALLBACK SciterWindowDelegate(HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID pParam, BOOL* handled);
// #elif defined(LINUX)
//   typedef LPVOID SciterWindowDelegate;
// #endif

// type HELEMENT C.HELEMENT

var (
	BAD_HELEMENT = C.HELEMENT(unsafe.Pointer(uintptr(0)))
)

var (
	elementHandlerMap = map[*Element]map[*EventHandler]struct{}{}
)

// Represents a single DOM element, owns and manages a Handle
type Element struct {
	handle C.HELEMENT
	err    error
	// defaultHandler *EventHandler
	*eventMapper
}

// Wrap C.HELEMENT to a go side *Element, doing Sciter_UseElement/Sciter_UnuseElement automatically
func WrapElement(he C.HELEMENT) *Element {
	e := &Element{
		handle: he,
	}
	e.use()
	runtime.SetFinalizer(e, (*Element).finalize)
	return e
}

// SCDOM_RESULT  Sciter_UseElement(HELEMENT he) ;//{ return SAPI()->Sciter_UseElement(he); }
func (e *Element) use() error {
	r := C.Sciter_UseElement(e.handle)
	return wrapDomResult(r, "Sciter_UseElement")
}

// SCDOM_RESULT  Sciter_UnuseElement(HELEMENT he) ;//{ return SAPI()->Sciter_UnuseElement(he); }

//Marks DOM object as unused (a.k.a. Release).
//
// UnUse would be called automatically when released by Go Runtime
// for *Element created by WrapElement(...)
//
//  Get handle of every element's child element.
//  \param[in] he \b #HELEMENT
//  \return \b #SCDOM_RESULT SCAPI
//
//  Application should call this function when it does not need element's
//  handle anymore.
//  \sa #Sciter_UseElement()
func (e *Element) unUse() error {
	r := C.Sciter_UnuseElement(e.handle)
	return wrapDomResult(r, "Sciter_UnuseElement")
}

// finalize() only happens when *Element is no longer used
// but the existence of elementHandlerMap makes that
// elements with EventHandler attached would not be gc collected
func (e *Element) finalize() {
	// Detach handlers
	// log.Printf("finalizing el 0x%x", e)
	// only detach when element does have handlers
	if handlersmap, ok := elementHandlerMap[e]; ok {
		for handler, _ := range handlersmap {
			e.DetachEventHandler(handler)
		}
	}
	// Release the underlying sciter handle
	e.unUse()
	e.handle = BAD_HELEMENT
}

// SCDOM_RESULT  SciterGetRootElement(HWINDOW hwnd, HELEMENT *phe) ;//{ return SAPI()->SciterGetRootElement(hwnd, phe); }

//Get root DOM element of HTML document.
//   \param[in] hwnd \b HWINDOW, Sciter window for which you need to get root
//   element
//   \param[out ] phe \b #HELEMENT*, variable to receive root element
//   \return \b #SCDOM_RESULT SCAPI
//
//   Root DOM object is always a 'HTML' element of the document.
func (s *Sciter) GetRootElement() (*Element, error) {
	var he C.HELEMENT
	r := C.SciterGetRootElement(s.hwnd, &he)
	return WrapElement(he), wrapDomResult(r, "SciterGetRootElement")
}

// SCDOM_RESULT  SciterGetFocusElement(HWINDOW hwnd, HELEMENT *phe) ;//{ return SAPI()->SciterGetFocusElement(hwnd, phe); }

//Get focused DOM element of HTML document.
//   \param[in] hwnd \b HWINDOW, Sciter window for which you need to get focus
//   element
//   \param[out ] phe \b #HELEMENT*, variable to receive focus element
//   \return \b #SCDOM_RESULT SCAPI
//
//   phe can have null value (0).
//
//   COMMENT: To set focus on element use SciterSetElementState(STATE_FOCUS,0)
func (s *Sciter) GetFocusElement() (*Element, error) {
	var he C.HELEMENT
	r := C.SciterGetFocusElement(s.hwnd, &he)
	return WrapElement(he), wrapDomResult(r, "SciterGetFocusElement")
}

// SCDOM_RESULT  SciterFindElement(HWINDOW hwnd, POINT pt, HELEMENT* phe) ;//{ return SAPI()->SciterFindElement(hwnd,pt,phe); }

// Find DOM element by coordinate.
//   \param[in] hwnd \b HWINDOW, Sciter window for which you need to find
//   elementz
//   \param[in] pt \b POINT, coordinates, window client area relative.
//   \param[out ] phe \b #HELEMENT*, variable to receive found element handle.
//   \return \b #SCDOM_RESULT SCAPI
//
//   If element was not found then *phe will be set to zero.
func (s *Sciter) FindElement(pt Point) (*Element, error) {
	var he C.HELEMENT
	cpt := *(*C.POINT)(unsafe.Pointer(&pt))
	r := C.SciterFindElement(s.hwnd, cpt, &he)
	return WrapElement(he), wrapDomResult(r, "SciterFindElement")
}

// SCDOM_RESULT  SciterGetChildrenCount(HELEMENT he, UINT* count) ;//{ return SAPI()->SciterGetChildrenCount(he, count); }

// Get number of child elements.
//   \param[in] he \b #HELEMENT, element which child elements you need to count
//   \param[out] count \b UINT*, variable to receive number of child elements
//   \return \b #SCDOM_RESULT SCAPI
//
//   \par Example:
//   for paragraph defined as
//   \verbatim <p>Hello <b>wonderfull</b> world!</p> \endverbatim
//   count will be set to 1 as the paragraph has only one sub element:
//   \verbatim <b>wonderfull</b> \endverbatim
func (e *Element) ChildrenCount() (int, error) {
	var count C.UINT
	r := C.SciterGetChildrenCount(e.handle, &count)
	return int(count), wrapDomResult(r, "SciterGetChildrenCount")
}

// SCDOM_RESULT  SciterGetNthChild(HELEMENT he, UINT n, HELEMENT* phe) ;//{ return SAPI()->SciterGetNthChild(he,n,phe); }

// Get handle of every element's child element.
//   \param[in] he \b #HELEMENT
//   \param[in] n \b UINT, number of the child element
//   \param[out] phe \b #HELEMENT*, variable to receive handle of the child
//   element
//   \return \b #SCDOM_RESULT SCAPI
//
//   \par Example:
//   for paragraph defined as
//   \verbatim <p>Hello <b>wonderfull</b> world!</p> \endverbatim
//   *phe will be equal to handle of &lt;b&gt; element:
//   \verbatim <b>wonderfull</b> \endverbatim
func (e *Element) NthChild(n int) (*Element, error) {
	var he C.HELEMENT
	r := C.SciterGetNthChild(e.handle, C.UINT(n), &he)
	return WrapElement(he), wrapDomResult(r, "SciterGetNthChild")
}

// SCDOM_RESULT  SciterGetParentElement(HELEMENT he, HELEMENT* p_parent_he) ;//{ return SAPI()->SciterGetParentElement(he,p_parent_he); }

// Get parent element.
func (e *Element) ParentElement() (*Element, error) {
	var he C.HELEMENT
	r := C.SciterGetParentElement(e.handle, &he)
	return WrapElement(he), wrapDomResult(r, "SciterGetParentElement")
}

// inline VOID SC_CALLBACK _LPCBYTE2ASTRING( LPCBYTE bytes, UINT num_bytes, LPVOID param )
// {
//     sciter::astring* s = (sciter::astring*)param;
//     *s = sciter::astring((const char*)bytes,num_bytes);
// }
// var _LPCSTR_RECEIVER = syscall.NewCallback(func(bs *byte, n uint, param uintptr) int {
//     s := (*string)(unsafe.Pointer(param))
//     *s = bytePtrToString(bs)
//     return 0
// })

// inline VOID SC_CALLBACK _LPCWSTR2STRING( LPCWSTR str, UINT str_length, LPVOID param )
// {
//     sciter::string* s = (sciter::string*)param;
//     *s = sciter::string(str,str_length);
// }
// var _LPCWSTR_RECEIVER = syscall.NewCallback(func(bs *uint16, n uint, param uintptr) int {
//     s := (*string)(unsafe.Pointer(param))
//     *s = utf16ToString(bs)
//     return 0
// })

// typedef VOID SC_CALLBACK LPCBYTE_RECEIVER( LPCBYTE bytes, UINT num_bytes, LPVOID param );

//export goLPCBYTE_RECEIVER
func goLPCBYTE_RECEIVER(bs *byte, n uint, param unsafe.Pointer) int {
	r := BytePtrToBytes(bs, n)
	*(*[]byte)(param) = r
	return 0
}

// typedef VOID SC_CALLBACK LPCWSTR_RECEIVER( LPCWSTR str, UINT str_length, LPVOID param );

//export goLPCWSTR_RECEIVER
func goLPCWSTR_RECEIVER(bs *uint16, n uint, param unsafe.Pointer) int {
	s := (*string)(param)
	*s = Utf16ToString(bs)
	return 0
}

// typedef VOID SC_CALLBACK LPCSTR_RECEIVER( LPCSTR str, UINT str_length, LPVOID param );

//export goLPCSTR_RECEIVER
func goLPCSTR_RECEIVER(bs C.LPCSTR, n uint, param unsafe.Pointer) int {
	s := (*string)(param)
	*s = C.GoString(bs)
	return 0
}

var (
	lpcbyte_receiver = (*C.LPCBYTE_RECEIVER)(unsafe.Pointer(C.LPCBYTE_RECEIVER_cgo))
	lpcwstr_receiver = (*C.LPCWSTR_RECEIVER)(unsafe.Pointer(C.LPCWSTR_RECEIVER_cgo))
	lpcstr_receiver  = (*C.LPCSTR_RECEIVER)(unsafe.Pointer(C.LPCSTR_RECEIVER_cgo))
)

// SCDOM_RESULT  SciterGetElementHtmlCB(HELEMENT he, BOOL outer, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterGetElementHtmlCB( he, outer, rcv, rcv_param); }

//Get html representation of the element.
//  \param[in] he \b #HELEMENT
//  \param[in] outer \b BOOL, if TRUE will retunr outer HTML otherwise inner.
//  \param[in] rcv \b pointer to function receiving UTF8 encoded HTML.
//  \param[in] rcv_param \b parameter that passed to rcv as it is.
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) Html(outer bool) (string, error) {
	var bs []byte
	// args
	couter := C.SBOOL(C.FALSE)
	if outer {
		couter = C.SBOOL(C.TRUE)
	}
	cparam := C.LPVOID(unsafe.Pointer(&bs))
	// cgo call
	r := C.SciterGetElementHtmlCB(e.handle, couter, lpcbyte_receiver, cparam)
	str := string(bs)
	return str, wrapDomResult(r, "SciterGetElementHtmlCB")
}

// SCDOM_RESULT  SciterGetElementTextCB(HELEMENT he, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param)

// Get inner text of the element as LPCWSTR (utf16 words).
//  \param[in] he \b #HELEMENT
//  \param[in] rcv \b pointer to the function receiving UTF16 encoded plain text
//  \param[in] rcv_param \b param passed that passed to LPCWSTR_RECEIVER "as is"
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) Text() (string, error) {
	var str string
	// args
	cparam := C.LPVOID(unsafe.Pointer(&str))
	// cgo call
	r := C.SciterGetElementTextCB(e.handle, lpcwstr_receiver, cparam)
	return str, wrapDomResult(r, "SciterGetElementTextCB")
}

// SCDOM_RESULT  SciterSetElementText(HELEMENT he, LPCWSTR utf16, UINT length)

//Set inner text of the element from LPCWSTR buffer (utf16 words).
//   \param[in] he \b #HELEMENT
//   \param[in] utf16words \b pointer, UTF16 encoded plain text
//   \param[in] length \b UINT, number of words in utf16words sequence
//   \return \b #SCDOM_RESULT SCAPI
func (e *Element) SetText(text string) error {
	// args
	u16, err := Utf16FromString(text)
	if err != nil {
		return err
	}
	clength := C.UINT(len(u16) - 1)
	ctext := C.LPCWSTR(unsafe.Pointer(&u16[0]))
	// cgo call
	r := C.SciterSetElementText(e.handle, ctext, clength)
	return wrapDomResult(r, "SciterSetElementText")
}

// SCDOM_RESULT  SciterGetAttributeCount(HELEMENT he, LPUINT p_count) ;//{ return SAPI()->SciterGetAttributeCount(he, p_count); }

//Get number of element's attributes.
//   \param[in] he \b #HELEMENT
//   \param[out] p_count \b LPUINT, variable to receive number of element
//   attributes.
//   \return \b #SCDOM_RESULT SCAPI
func (e *Element) AttrCount() (int, error) {
	var count C.UINT
	// cgo call
	r := C.SciterGetAttributeCount(e.handle, &count)
	return int(count), wrapDomResult(r, "SciterGetAttributeCount")
}

// SCDOM_RESULT  SciterGetNthAttributeNameCB(HELEMENT he, UINT n, LPCSTR_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterGetNthAttributeNameCB(he,n,rcv,rcv_param); }

func (e *Element) NthAttrName(n int) (name string, err error) {
	// args
	cn := C.UINT(n)
	cparam := C.LPVOID(unsafe.Pointer(&name))
	// cgo call
	r := C.SciterGetNthAttributeNameCB(e.handle, cn, lpcstr_receiver, cparam)
	err = wrapDomResult(r, "SciterGetNthAttributeNameCB")
	return
}

// SCDOM_RESULT  SciterGetNthAttributeValueCB(HELEMENT he, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterGetNthAttributeValueCB(he, n, rcv, rcv_param); }

//Get value of any element's attribute by attribute's number.
//  \param[in] he \b #HELEMENT
//  \param[in] n \b UINT, number of desired attribute
//  \param[out] p_name \b LPCSTR*, will be set to address of the string
//  containing attribute name
//  \param[out] p_value \b LPCWSTR*, will be set to address of the string
//  containing attribute value
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) NthAttr(n int) (value string, err error) {
	// args
	cn := C.UINT(n)
	cparam := C.LPVOID(unsafe.Pointer(&value))
	// cgo call
	r := C.SciterGetNthAttributeValueCB(e.handle, cn, lpcwstr_receiver, cparam)
	err = wrapDomResult(r, "SciterGetNthAttributeValueCB")
	return
}

// SCDOM_RESULT  SciterGetAttributeByNameCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterGetAttributeByNameCB(he,name,rcv,rcv_param); }

//Get value of any element's attribute by name.
//  \param[in] he \b #HELEMENT
//  \param[in] name \b LPCSTR, attribute name
//  \param[out] p_value \b LPCWSTR*, will be set to address of the string
//  containing attribute value
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) Attr(name string) (string, error) {
	var str string
	// args
	cname := C.CString(name)
	crcv := (*C.LPCWSTR_RECEIVER)(unsafe.Pointer(C.LPCWSTR_RECEIVER_cgo))
	cparam := C.LPVOID(unsafe.Pointer(&str))
	// cgo call
	r := C.SciterGetAttributeByNameCB(e.handle, cname, crcv, cparam)
	C.free(unsafe.Pointer(cname))
	return str, wrapDomResult(r, "SciterGetAttributeByNameCB")
}

// SCDOM_RESULT  SciterSetAttributeByName(HELEMENT he, LPCSTR name, LPCWSTR value) ;//{ return SAPI()->SciterSetAttributeByName(he,name,value); }

//Set attribute's value.
//  \param[in] he \b #HELEMENT
//  \param[in] name \b LPCSTR, attribute name
//  \param[in] value \b LPCWSTR, new attribute value or 0 if you want to remove attribute.
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) SetAttr(name, val string) error {
	// args
	cname := C.CString(name)
	cval := C.LPCWSTR(StringToWcharPtr(val))
	// cgo call
	r := C.SciterSetAttributeByName(e.handle, cname, cval)
	C.free(unsafe.Pointer(cname))
	return wrapDomResult(r, "SciterSetAttributeByName")
}

// SCDOM_RESULT  SciterClearAttributes(HELEMENT he)

//Remove all attributes from the element.
//  \param[in] he \b #HELEMENT
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) ClearAttr() error {
	return wrapDomResult(C.SciterClearAttributes(e.handle), "SciterClearAttributes")
}

// SCDOM_RESULT  SciterGetElementIndex(HELEMENT he, LPUINT p_index)

//Get element index.
//  \param[in] he \b #HELEMENT
//  \param[out] p_index \b LPUINT, variable to receive number of the element
//  among parent element's subelements.
//  \return \b #SCDOM_RESULT SCAPI
func (e *Element) Index() (idx int, err error) {
	// args
	var cidx C.UINT
	// cgo call
	r := C.SciterGetElementIndex(e.handle, &cidx)
	idx = int(cidx)
	err = wrapDomResult(r, "SciterGetElementIndex")
	return
}

// SCDOM_RESULT  SciterGetElementType(HELEMENT he, LPCSTR* p_type)
// SCDOM_RESULT  SciterGetElementTypeCB(HELEMENT he, LPCSTR_RECEIVER* rcv, LPVOID rcv_param)

// Get element's type.
func (e *Element) Type() (string, error) {
	var str string
	// args
	cparam := C.LPVOID(unsafe.Pointer(&str))
	// cgo call
	r := C.SciterGetElementTypeCB(e.handle, lpcstr_receiver, cparam)
	return str, wrapDomResult(r, "SciterGetElementTypeCB")
}

// SCDOM_RESULT  SciterGetStyleAttributeCB(HELEMENT he, LPCSTR name, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterGetStyleAttributeCB(he,name,rcv,rcv_param); }

//Get element's style attribute.
//  \param[in] he \b #HELEMENT
//  \param[in] name \b LPCSTR, name of the style attribute
//  \param[in] rcv \b pointer to the function receiving UTF16 encoded plain text
//  \param[in] rcv_param \b param passed that passed to LPCWSTR_RECEIVER "as is"
//
//  Style attributes are those that are set using css. E.g. "font-face: arial" or "display: block".
//
//  \sa #SciterSetStyleAttribute()
func (e *Element) Style(name string) (string, error) {
	var str string
	// args
	cname := C.CString(name)
	crcv := (*C.LPCWSTR_RECEIVER)(unsafe.Pointer(C.LPCWSTR_RECEIVER_cgo))
	cparam := C.LPVOID(unsafe.Pointer(&str))
	// cgo call
	r := C.SciterGetStyleAttributeCB(e.handle, cname, crcv, cparam)
	C.free(unsafe.Pointer(cname))
	return str, wrapDomResult(r, "SciterGetStyleAttributeCB")
}

// SCDOM_RESULT  SciterSetStyleAttribute(HELEMENT he, LPCSTR name, LPCWSTR value) ;//{ return SAPI()->SciterSetStyleAttribute(he,name,value); }

func (e *Element) SetStyle(name, val string) error {
	// args
	cname := C.CString(name)
	cval := C.LPCWSTR(StringToWcharPtr(val))
	// cgo call
	r := C.SciterSetStyleAttribute(e.handle, cname, cval)
	C.free(unsafe.Pointer(cname))
	return wrapDomResult(r, "SciterSetStyleAttribute")
}

// SCDOM_RESULT  SciterGetElementLocation(HELEMENT he, LPRECT p_location, UINT areas /*ELEMENT_AREAS*/) ;//{ return SAPI()->SciterGetElementLocation(he,p_location,areas); }

// SCDOM_RESULT  SciterScrollToView(HELEMENT he, UINT SciterScrollFlags) ;//{ return SAPI()->SciterScrollToView(he,SciterScrollFlags); }

// Scroll to view.
func (e *Element) ScrollToView(flag SCITER_SCROLL_FLAGS) error {
	// args
	cflag := C.UINT(flag)
	// cgo call
	r := C.SciterScrollToView(e.handle, cflag)
	return wrapDomResult(r, "SciterScrollToView")
}

// SCDOM_RESULT  SciterUpdateElement(HELEMENT he, BOOL andForceRender)

// Apply changes and refresh element area in its window.
//   \param[in] he \b #HELEMENT
//   \param[in] andForceRender \b BOOL, TRUE to force UpdateWindow() call.
//   \return \b #SCDOM_RESULT SCAPI
func (e *Element) Update(forceRender bool) error {
	// args
	var cforceRender C.SBOOL = C.FALSE
	if forceRender {
		cforceRender = C.TRUE
	}
	// cgo call
	return wrapDomResult(C.SciterUpdateElement(e.handle, cforceRender), "SciterUpdateElement")
}

// SCDOM_RESULT  SciterRefreshElementArea(HELEMENT he, RECT rc)

// SCDOM_RESULT  SciterSetCapture(HELEMENT he)

//Set the mouse capture to the specified element.
//  \param[in] he \b #HELEMENT
//  \return \b #SCDOM_RESULT SCAPI
//
//  After call to this function all mouse events will be targeted to the element.
//  To remove mouse capture call ReleaseCapture() function.
func (e *Element) Capture() error {
	return wrapDomResult(C.SciterSetCapture(e.handle), "SciterSetCapture")
}

// SCDOM_RESULT  SciterReleaseCapture(HELEMENT he)
func (e *Element) ReleaseCapture() error {
	return wrapDomResult(C.SciterReleaseCapture(e.handle), "SciterReleaseCapture")
}

// SCDOM_RESULT  SciterGetElementHwnd(HELEMENT he, HWINDOW* p_hwnd, BOOL rootWindow)

// Get HWINDOW of containing window.
func (e *Element) GetHwnd(rootWindow bool) (hwnd C.HWINDOW, err error) {
	// args
	var crootWindow C.SBOOL = C.FALSE
	if rootWindow {
		crootWindow = C.TRUE
	}
	// cgo call
	r := C.SciterGetElementHwnd(e.handle, (C.HWINDOW_PTR)(unsafe.Pointer(&hwnd)), crootWindow)
	err = wrapDomResult(r, "SciterGetElementHwnd")
	return
}

// SCDOM_RESULT  SciterCombineURL(HELEMENT he, LPWSTR szUrlBuffer, UINT UrlBufferSize)

// Combine given URL with URL of the document element belongs to.
//   \param[in] he \b #HELEMENT
//   \param[in, out] szUrlBuffer \b LPWSTR, at input this buffer contains
//   zero-terminated URL to be combined, after function call it contains
//   zero-terminated combined URL
//   \param[in] UrlBufferSize \b UINT, size of the buffer pointed by
//   \c szUrlBuffer
//   \return \b #SCDOM_RESULT SCAPI
//
//   This function is used for resolving relative references.
func (e *Element) CombineURL(url string) (combinedUrl string, err error) {
	var buf []uint16
	buf, err = Utf16FromString(url)
	if err != nil {
		return
	}
	buf = append(buf, make([]uint16, 1024)...)
	// args
	cBuffer := C.LPWSTR(unsafe.Pointer(&buf[0]))
	cBufferSize := C.UINT(len(buf))
	// cgo call
	r := C.SciterCombineURL(e.handle, cBuffer, cBufferSize)
	if SCDOM_RESULT(r) != SCDOM_OK {
		return "", wrapDomResult(r, "SciterCombineURL")
	}
	// get combinedUrl
	combinedUrl = Utf16ToString(&buf[0])
	return
}

// SCDOM_RESULT  SciterSelectElements(HELEMENT  he, LPCSTR    CSS_selectors, SciterElementCallback* callback, LPVOID param)

// typedef BOOL SC_CALLBACK SciterElementCallback( HELEMENT he, LPVOID param );

//export goSciterElementCallback
func goSciterElementCallback(he C.HELEMENT, param unsafe.Pointer) int {
	idx := int(uintptr(param))
	e := WrapElement(he)
	globalElementCallbackResults[idx] = append(globalElementCallbackResults[idx], e)
	return 0
}

var (
	sciterElementCallback        = (*C.SciterElementCallback)(unsafe.Pointer(C.SciterElementCallback_cgo))
	globalElementCallbackResults = make([][]*Element, 0)
)

// SCDOM_RESULT  SciterSelectElementsW(HELEMENT  he, LPCWSTR   CSS_selectors, SciterElementCallback* callback, LPVOID param)
func (e *Element) Select(css_selectors string) ([]*Element, error) {
	results := make([]*Element, 0, 32)
	globalElementCallbackResults = append(globalElementCallbackResults, results)
	idx := len(globalElementCallbackResults) - 1
	// args
	cselectors := C.LPCWSTR(StringToWcharPtr(css_selectors))
	cparam := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	// cgo call
	r := C.SciterSelectElementsW(e.handle, cselectors, sciterElementCallback, cparam)
	err := wrapDomResult(r, "SciterSelectElementsW")
	results = globalElementCallbackResults[idx]
	globalElementCallbackResults[idx] = nil
	return results, err
}

// Returns the only child element that matches the selector.
func (e *Element) SelectFirst(css_selectors string) (*Element, error) {
	els, err := e.Select(css_selectors)
	if err != nil {
		return nil, fmt.Errorf("%s:%s", "SelectFirst", "no proper element found")
	}
	if len(els) == 0 {
		return nil, nil
	}
	return els[0], nil
}

// Returns the only child element that matches the selector.
// If no elements match or more than one element matches, the function returns error.
func (e *Element) SelectUnique(selector string) (*Element, error) {
	els, err := e.Select(selector)
	if err != nil || len(els) != 1 {
		return nil, fmt.Errorf("%s:%s", "SelectUnique", "no unique element found")
	}
	return els[0], nil
}

// Returns the only child element that matches the selector.
// If no elements match or more than one element matches, the function panics.
func (e *Element) MustSelectUnique(selector string) *Element {
	r, err := e.SelectUnique(selector)
	if err != nil {
		log.Panic(err)
	}
	return r
}

// A wrapper of SelectUnique that auto-prepends a hash to the provided id.
// Useful when selecting elements base on a programmatically retrieved id (which does
// not already have the hash on it)
func (e *Element) SelectById(id string) (*Element, error) {
	return e.SelectUnique(fmt.Sprintf("#%s", id))
}

func (e *Element) MustSelectById(id string) *Element {
	return e.MustSelectUnique(fmt.Sprintf("#%s", id))
}

// SCDOM_RESULT  SciterSelectParent(HELEMENT  he, LPCSTR    selector, UINT      depth, HELEMENT* heFound)

// SCDOM_RESULT  SciterSelectParentW(HELEMENT  he, LPCWSTR   selector, UINT      depth, HELEMENT* heFound)
func (e *Element) SelectParent(css_selector string, depth int) (*Element, error) {
	var heFound C.HELEMENT
	// args
	cselector := C.LPCWSTR(StringToWcharPtr(css_selector))
	cdepth := C.UINT(depth)
	// cgo call
	r := C.SciterSelectParentW(e.handle, cselector, cdepth, &heFound)
	return WrapElement(heFound), wrapDomResult(r, "SciterSelectParentW")
}

// SCDOM_RESULT  SciterSetElementHtml(HELEMENT he, const BYTE* html, UINT htmlLength, UINT where)
func (e *Element) SetHtml(html string, where SET_ELEMENT_HTML) error {
	if len(html) == 0 {
		return e.Clear()
	}

	// args
	chtml := (*C.BYTE)(StringToBytePtr(html))
	clen := C.UINT(len(html))
	cwhere := C.UINT(where)
	// cgo call
	r := C.SciterSetElementHtml(e.handle, chtml, clen, cwhere)
	return wrapDomResult(r, "SciterSetElementHtml")
}

// SCDOM_RESULT  SciterGetElementUID(HELEMENT he, UINT* puid)
func (e *Element) UID() (uint, error) {
	// args
	var uid C.UINT
	// cgo call
	r := C.SciterGetElementUID(e.handle, &uid)
	return uint(uid), wrapDomResult(r, "SciterGetElementUID")
}

// SCDOM_RESULT  SciterGetElementByUID(HWINDOW hwnd, UINT uid, HELEMENT* phe)
func (s *Sciter) GetElementByUID(uid uint) (*Element, error) {
	// args
	cuid := C.UINT(uid)
	var he C.HELEMENT
	// cgo call
	r := C.SciterGetElementByUID(s.hwnd, cuid, &he)
	return WrapElement(he), wrapDomResult(r, "SciterGetElementByUID")
}

// SCDOM_RESULT  SciterShowPopup(HELEMENT hePopup, HELEMENT heAnchor, UINT placement)

//Shows block element (DIV) in popup window.
// \param[in] hePopup \b HELEMENT, element to show as popup
// \param[in] heAnchor \b HELEMENT, anchor element - hePopup will be shown near this element
// \param[in] placement \b UINT, values:
//     2 - popup element below of anchor
//     8 - popup element above of anchor
//     4 - popup element on left side of anchor
//     6 - popup element on right side of anchor
//     ( see numpad on keyboard to get an idea of the numbers)
// \return \b #SCDOM_RESULT SCAPI
func (e *Element) ShowPopup(eAnchor *Element, placement PopupPlacement) error {
	// args
	cplacement := C.UINT(placement)
	// cgo call
	r := C.SciterShowPopup(e.handle, eAnchor.handle, cplacement)
	return wrapDomResult(r, "SciterShowPopup")
}

// SCDOM_RESULT  SciterShowPopupAt(HELEMENT hePopup, POINT pos, BOOL animate)

// Shows block element (DIV) in popup window at given position.
//  \param[in] hePopup \b HELEMENT, element to show as popup
//  \param[in] pos \b POINT, popup element position, relative to origin of Sciter window.
//  \param[in] placement \b UINT, values:
//      2 - popup element below of anchor
//      8 - popup element above of anchor
//      4 - popup element on left side of anchor
//      6 - popup element on right side of anchor
//      ( see numpad on keyboard to get an idea of the numbers)
func (e *Element) ShowPopupAt(pos Point, placement uint) error {
	// args
	var cpos C.POINT = *(*C.POINT)(unsafe.Pointer(&pos))
	cplacement := C.UINT(placement)
	// cgo call
	r := C.SciterShowPopupAt(e.handle, cpos, cplacement)
	return wrapDomResult(r, "SciterShowPopupAt")
}

// SCDOM_RESULT  SciterHidePopup(HELEMENT he)
func (e *Element) HidePopup() error {
	// cgo call
	r := C.SciterHidePopup(e.handle)
	return wrapDomResult(r, "SciterHidePopup")
}

// SCDOM_RESULT  SciterGetElementState( HELEMENT he, UINT* pstateBits)

// Get/set state bits, stateBits*** accept or'ed values above
func (e *Element) State() (ElementState, error) {
	// args
	var state C.UINT
	// cgo call
	r := C.SciterGetElementState(e.handle, &state)
	return ElementState(state), wrapDomResult(r, "SciterGetElementState")
}

// SCDOM_RESULT  SciterSetElementState( HELEMENT he, UINT stateBitsToSet, UINT stateBitsToClear, BOOL updateView)
func (e *Element) SetState(bitsToSet, bitsToClear ElementState, updateView bool) error {
	// args
	cbitsToSet := C.UINT(bitsToSet)
	cbitsToClear := C.UINT(bitsToClear)
	var cupdateView C.SBOOL = C.FALSE
	if updateView {
		cupdateView = C.TRUE
	}
	// cgo call
	r := C.SciterSetElementState(e.handle, cbitsToSet, cbitsToClear, cupdateView)
	return wrapDomResult(r, "SciterSetElementState")
}

// SCDOM_RESULT  SciterCreateElement( LPCSTR tagname, LPCWSTR textOrNull, /*out*/ HELEMENT *phe )

// Create new element, the element is disconnected initially from the DOM.
//     Element created with ref_count = 1 thus you \b must call Sciter_UnuseElement on returned handler.
//  \param[in] tagname \b LPCSTR, html tag of the element e.g. "div", "option", etc.
//  \param[in] textOrNull \b LPCWSTR, initial text of the element or NULL. text here is a plain text - method does no parsing.
//  \param[out ] phe \b #HELEMENT*, variable to receive handle of the element
func CreateElement(tagname, textOrNull string) (*Element, error) {
	// args
	var he C.HELEMENT
	ctagname := C.LPCSTR(unsafe.Pointer(StringToBytePtr(tagname)))
	ctextOrNull := C.LPCWSTR(StringToWcharPtr(textOrNull))
	// cgo call
	r := C.SciterCreateElement(ctagname, ctextOrNull, &he)
	return WrapElement(he), wrapDomResult(r, "SciterCreateElement")
}

// SCDOM_RESULT  SciterCloneElement( HELEMENT he, /*out*/ HELEMENT *phe )

// Create new element as copy of existing element, new element is a full (deep) copy of the element and
//     is disconnected initially from the DOM.
//     Element created with ref_count = 1 thus you \b must call Sciter_UnuseElement on returned handler.
//  \param[in] he \b #HELEMENT, source element.
//  \param[out ] phe \b #HELEMENT*, variable to receive handle of the new element.
func (e *Element) Clone() (*Element, error) {
	// args
	var he C.HELEMENT
	// cgo call
	r := C.SciterCloneElement(e.handle, &he)
	return WrapElement(he), wrapDomResult(r, "SciterCloneElement")
}

// SCDOM_RESULT  SciterInsertElement( HELEMENT he, HELEMENT hparent, UINT index )
//
// Insert element at the index position of parent.
//     It is not an error to insert element which already has parent - it will be disconnected first, but
//     you need to update elements parent in this case.
func (e *Element) Insert(el *Element, index int) error {
	// args
	cindex := C.UINT(index)
	// cgo call
	r := C.SciterInsertElement(el.handle, e.handle, cindex)
	return wrapDomResult(r, "SciterInsertElement")
}

// Append element e as last child of this element.
func (e *Element) Append(el *Element) error {
	// args
	cindex := C.UINT(0x7FFFFFFF)
	// cgo call
	r := C.SciterInsertElement(el.handle, e.handle, cindex)
	return wrapDomResult(r, "Element.Append")
}

// SCDOM_RESULT  SciterDetachElement( HELEMENT he )

// Take element out of its container (and DOM tree).
//    Element will be destroyed when its reference counter will become zero
func (e *Element) Detach() error {
	// cgo call
	r := C.SciterDetachElement(e.handle)
	return wrapDomResult(r, "SciterDetachElement")
}

// SCDOM_RESULT  SciterDeleteElement(HELEMENT he)

// Take element out of its container (and DOM tree).
//   Element will be destroyed when its reference counter will become zero
func (e *Element) Delete() error {
	// cgo cal
	r := C.SciterDeleteElement(e.handle)
	return wrapDomResult(r, "SciterDeleteElement")
}

// Clear contents and all children of the element.
func (e *Element) Clear() error {
	// cgo cal
	r := C.SciterSetElementText(e.handle, C.LPCWSTR(nil), C.UINT(0))
	return wrapDomResult(r, "Element.Clear")
}

// SCDOM_RESULT  SciterSetTimer( HELEMENT he, UINT milliseconds, UINT_PTR timer_id )

// Start Timer for the element.
//   Element will receive on_timer event
//   To stop timer call SciterSetTimer( he, 0 );
func (e *Element) SetTimer(milliseconds int) error {
	// args
	cmilliseconds := C.UINT(milliseconds)
	ctimeer_id := (C.UINT_PTR)(0)
	// cgo call
	r := C.SciterSetTimer(e.handle, cmilliseconds, ctimeer_id)
	return wrapDomResult(r, "SciterSetTimer")
}

// typedef BOOL SC_CALLBACK ElementEventProc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms );
// typedef ElementEventProc* LPELEMENT_EVENT_PROC;

// Main event handler that dispatches to the right element handler

var (
	// Hold a reference to handlers that are in-use so that they don't
	// get garbage collected.
	globalEventHandlers = make([]*EventHandler, 0)
	behaviors           = make(map[*EventHandler]int, 32)
)

const (
	handlerNotFound = -1
)

// -1 indacates not found
func findEventHandlerIdx(eh *EventHandler) int {
	for i, v := range globalEventHandlers {
		if v == eh {
			return i
		}
	}
	return handlerNotFound
}

//export goElementEventProc
func goElementEventProc(tag unsafe.Pointer, he C.HELEMENT, evtg uint, params unsafe.Pointer) int {
	handler := globalEventHandlers[int(uintptr(tag))]
	handled := false
	// el := WrapElement(he)
	el := WrapElement(he)

	switch evtg {
	case SUBSCRIPTIONS_REQUEST:
		p := (*uint32)(params)
		*p = HANDLE_ALL
		handled = true
	case HANDLE_INITIALIZATION:
		if p := (*InitializationParams)(params); p.Cmd == BEHAVIOR_ATTACH {
			//log.Print("Attach event handler to ", NewElementFromHandle(HELEMENT(he)).Describe())
			if handler.OnAttached != nil {
				handler.OnAttached(el)
			}
		} else if p.Cmd == BEHAVIOR_DETACH {
			//log.Print("Detach event handler from ", NewElementFromHandle(el).Describe())
			if handler.OnDetached != nil {
				handler.OnDetached(el)
			}

			// If this was a behavior detaching, decrement the reference count and stop tracking
			// the pointer if the ref count has been exhausted
			if behaviorRefCount, exists := behaviors[handler]; exists {
				behaviorRefCount--
				if behaviorRefCount == 0 {
					delete(behaviors, handler)
				} else {
					behaviors[handler] = behaviorRefCount
				}
			}
		}
		handled = true
	case HANDLE_MOUSE:
		if handler.OnMouse != nil {
			p := (*MouseParams)(params)
			handled = handler.OnMouse(el, p)
		}
	case HANDLE_KEY:
		if handler.OnKey != nil {
			p := (*KeyParams)(params)
			handled = handler.OnKey(el, p)
		}
	case HANDLE_FOCUS:
		if handler.OnFocus != nil {
			p := (*FocusParams)(params)
			handled = handler.OnFocus(el, p)
		}
	case HANDLE_DRAW:
		if handler.OnDraw != nil {
			p := (*DrawParams)(params)
			handled = handler.OnDraw(el, p)
		}
	case HANDLE_TIMER:
		if handler.OnTimer != nil {
			p := (*TimerParams)(params)
			handled = handler.OnTimer(el, p)
		}
	case HANDLE_BEHAVIOR_EVENT:
		if handler.OnBehaviorEvent != nil {
			p := (*BehaviorEventParams)(params)
			handled = handler.OnBehaviorEvent(el, p)
		}
	case HANDLE_METHOD_CALL:
		if handler.OnMethodCall != nil {
			p := (*MethodParams)(params)
			handled = handler.OnMethodCall(el, p)
		}
	case HANDLE_SCRIPTING_METHOD_CALL:
		if handler.OnScriptingMethodCall != nil {
			p := (*ScriptingMethodParams)(params)
			handled = handler.OnScriptingMethodCall(el, p)
		}
	case HANDLE_TISCRIPT_METHOD_CALL:
		if handler.OnTiscriptMethodCall != nil {
			p := (*TiscriptMethodParams)(params)
			handled = handler.OnTiscriptMethodCall(el, p)
		}

	case HANDLE_DATA_ARRIVED:
		if handler.OnDataArrived != nil {
			p := (*DataArrivedParams)(params)
			handled = handler.OnDataArrived(el, p)
		}
	case HANDLE_SIZE:
		if handler.OnSize != nil {
			handler.OnSize(el)
		}
	case HANDLE_SCROLL:
		if handler.OnScroll != nil {
			p := (*ScrollParams)(params)
			handled = handler.OnScroll(el, p)
		}
	case HANDLE_EXCHANGE:
		if handler.OnExchange != nil {
			p := (*ExchangeParams)(params)
			handled = handler.OnExchange(el, p)
		}
	case HANDLE_GESTURE:
		if handler.OnGesture != nil {
			p := (*GestureParams)(params)
			handled = handler.OnGesture(el, p)
		}
	case HANDLE_SOM:
		if handler.OnSom != nil {
			p := (*SomParams)(params)
			handled = handler.OnSom(el, p)
		}
	default:
		log.Panicf("Unhandled sciter event case: 0x%04X.\nCheck `EVENT_GROUPS` in sciter-x-behavior.h in the latest Sciter SDK", evtg)
	}
	if handled {
		return 1
	}
	return 0
}

var (
	element_event_proc = (C.LPELEMENT_EVENT_PROC)(unsafe.Pointer(C.ElementEventProc_cgo))
)

// SCDOM_RESULT  SciterDetachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag )
func (e *Element) DetachEventHandler(handler *EventHandler) error {
	// test
	hm, ok := elementHandlerMap[e]
	if !ok {
		hm = make(map[*EventHandler]struct{}, 0)
		elementHandlerMap[e] = hm
	}
	if _, exists := hm[handler]; !exists {
		return nil
	}
	// args
	idx := findEventHandlerIdx(handler)
	tag := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	// cgo call
	if ret := C.SciterDetachEventHandler(e.handle, element_event_proc, tag); SCDOM_RESULT(ret) != SCDOM_OK {
		return wrapDomResult(ret, "SciterDetachEventHandler")
	} else {
		globalEventHandlers[idx] = nil
		delete(hm, handler)
	}
	return nil
}

func (e *Element) isValid() bool {
	return e.handle != BAD_HELEMENT
}

// SCDOM_RESULT  SciterAttachEventHandler( HELEMENT he, LPELEMENT_EVENT_PROC pep, LPVOID tag ) ;//{ return SAPI()->SciterAttachEventHandler( he,pep,tag ); }
//
// Any Element that calls this function would not be gc collected any more
// thus prevent the handler missing in sciter callbacks
func (e *Element) AttachEventHandler(handler *EventHandler) error {
	hm, ok := elementHandlerMap[e]
	if !ok {
		hm = make(map[*EventHandler]struct{}, 0)
		elementHandlerMap[e] = hm
	}
	// allready attached
	if _, exists := hm[handler]; exists {
		return nil
	}
	// args
	globalEventHandlers = append(globalEventHandlers, handler)
	idx := len(globalEventHandlers) - 1
	tag := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	// do attach
	hm[handler] = struct{}{}
	// Don't let the caller disable ATTACH/DETACH events, otherwise we
	// won't know when to throw out our event handler object
	if ret := C.SciterAttachEventHandler(e.handle, element_event_proc, tag); SCDOM_RESULT(ret) != SCDOM_OK {
		return wrapDomResult(ret, "SciterAttachEventHandler")
	}
	return nil
}

// This is the same as AttachHandler, except that behaviors are singleton instances stored
// in a master map.  They may be shared among many elements since they have no state.
// The only reason we keep a separate set of the behaviors is so that the event handler
// dispatch method can tell if an event handler is a behavior or a regular handler.
func (e *Element) attachBehavior(handler *EventHandler) {
	if !e.isValid() {
		return
	}
	e.AttachEventHandler(handler)
}

// SCDOM_RESULT  SciterWindowAttachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag, UINT subscription )
func (s *Sciter) AttachWindowEventHandler(handler *EventHandler) error {
	// prevent duplicated attachement
	for _, v := range globalEventHandlers {
		if v == handler {
			return nil
		}
	}
	// new attach
	// args
	globalEventHandlers = append(globalEventHandlers, handler)
	idx := len(globalEventHandlers) - 1
	tag := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	// // detach first
	// s.DetachWindowEventHandler()

	// // Overwrite if it exists
	// s.WindowEventHandler = handler

	// Don't let the caller disable ATTACH/DETACH events, otherwise we
	// won't know when to throw out our event handler object
	// subscription := handler.Subscription()
	// subscription &= ^uint(DISABLE_INITIALIZATION & 0xffffffff)

	r := C.SciterWindowAttachEventHandler(s.hwnd, element_event_proc, tag, HANDLE_ALL)
	return wrapDomResult(r, "SciterWindowAttachEventHandler")
}

// SCDOM_RESULT  SciterWindowDetachEventHandler( HWINDOW hwndLayout, LPELEMENT_EVENT_PROC pep, LPVOID tag )
func (s *Sciter) DetachWindowEventHandler(handler *EventHandler) error {
	// if s.WindowEventHandler != nil {
	idx := findEventHandlerIdx(handler)
	if idx == handlerNotFound {
		return nil
	}
	tag := C.LPVOID(unsafe.Pointer(uintptr(idx)))
	ret := C.SciterWindowDetachEventHandler(s.hwnd, element_event_proc, tag)
	globalEventHandlers[idx] = nil
	return wrapDomResult(ret, "SciterWindowDetachEventHandler")
}

// SCDOM_RESULT  SciterSendEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason, /*out*/ BOOL* handled) ;//{ return SAPI()->SciterSendEvent( he,appEventCode,heSource,reason,handled); }
/** SendEvent - sends sinking/bubbling event to the child/parent chain of he element.
   First event will be send in SINKING mode (with SINKING flag) - from root to he element itself.
   Then from he element to its root on parents chain without SINKING flag (bubbling phase).

* \param[in] he \b HELEMENT, element to send this event to.
* \param[in] appEventCode \b UINT, event ID, see: #BEHAVIOR_EVENTS
* \param[in] heSource \b HELEMENT, optional handle of the source element, e.g. some list item
* \param[in] reason \b UINT, notification specific event reason code
* \param[out] handled \b BOOL*, variable to receive TRUE if any handler handled it, FALSE otherwise.

**/

// SCDOM_RESULT  SciterPostEvent( HELEMENT he, UINT appEventCode, HELEMENT heSource, UINT reason) ;//{ return SAPI()->SciterPostEvent(he,appEventCode,heSource,reason); }
/** PostEvent - post sinking/bubbling event to the child/parent chain of he element.
 *  Function will return immediately posting event into input queue of the application.
 *
 * \param[in] he \b HELEMENT, element to send this event to.
 * \param[in] appEventCode \b UINT, event ID, see: #BEHAVIOR_EVENTS
 * \param[in] heSource \b HELEMENT, optional handle of the source element, e.g. some list item
 * \param[in] reason \b UINT, notification specific event reason code

 **/

// SCDOM_RESULT  SciterFireEvent( const BEHAVIOR_EVENT_PARAMS* evt, BOOL post, BOOL *handled) ;//{ return SAPI()->SciterFireEvent( evt, post, handled ); }
// SCDOM_RESULT  SciterCallBehaviorMethod(HELEMENT he, struct METHOD_PARAMS* params) ;//{ return SAPI()->SciterCallBehaviorMethod(he,params); }
/** SciterCallMethod - calls behavior specific method.
 * \param[in] he \b HELEMENT, element - source of the event.
 * \param[in] params \b METHOD_PARAMS, pointer to method param block
 **/

// SCDOM_RESULT  SciterRequestElementData( HELEMENT he, LPCWSTR url, UINT dataType, HELEMENT initiator ) ;//{ return SAPI()->SciterRequestElementData(he,url,dataType,initiator ); }

// SciterRequestElementData  - request data download for the element. as jQuery.load :)
//  \param[in] he \b HELEMENT, element to deleiver data to.
//  \param[in] url \b LPCWSTR, url to download data from.
//  \param[in] dataType \b UINT, data type, see SciterResourceType.
//  \param[in] hInitiator \b HELEMENT, element - initiator, can be NULL.
//
//  event handler on the he element (if any) will be notified
//  when data will be ready by receiving HANDLE_DATA_DELIVERY event.
func (e *Element) Load(url string, dataType SciterResourceType) error {
	// args
	curl := C.LPCWSTR(StringToWcharPtr(url))
	cdataType := C.UINT(dataType)
	hInitiator := BAD_HELEMENT
	// cgo call
	r := C.SciterRequestElementData(e.handle, curl, cdataType, hInitiator)
	return wrapDomResult(r, "SciterRequestElementData")
}

// SCDOM_RESULT  SciterHttpRequest( HELEMENT he, LPCWSTR url, UINT dataType, UINT requestType, struct REQUEST_PARAM* requestParams, UINT nParams)

// SciterSendRequest - send GET or POST request for the element
//
//  event handler on the 'he' element (if any) will be notified
//  when data will be ready by receiving HANDLE_DATA_DELIVERY event.
func (e *Element) HttpRequest(url string, dataType SciterResourceType, requestType RequestType, params ...RequestParam) error {
	// args
	curl := C.LPCWSTR(StringToWcharPtr(url))
	cdataType := C.UINT(dataType)
	crequestType := C.UINT(requestType)
	nParams := len(params)
	cnParams := C.UINT(nParams)
	rParams := make([]C.REQUEST_PARAM, nParams)
	for i := 0; i < nParams; i++ {
		rParams[i].name = C.LPCWSTR(StringToWcharPtr(params[i].Name))
		rParams[i].value = C.LPCWSTR(StringToWcharPtr(params[i].Value))
	}
	var crParams *C.REQUEST_PARAM
	if nParams > 0 {
		crParams = (*C.REQUEST_PARAM)(unsafe.Pointer(&rParams[0]))
	}
	// cgo call
	r := C.SciterHttpRequest(e.handle, curl, cdataType, crequestType, crParams, cnParams)
	return wrapDomResult(r, "SciterHttpRequest")
}

// SCDOM_RESULT  SciterGetScrollInfo( HELEMENT he, LPPOINT scrollPos, LPRECT viewRect, LPSIZE contentSize ) ;//{ return SAPI()->SciterGetScrollInfo( he,scrollPos,viewRect,contentSize ); }
// SCDOM_RESULT  SciterSetScrollPos( HELEMENT he, POINT scrollPos, BOOL smooth ) ;//{ return SAPI()->SciterSetScrollPos( he,scrollPos,smooth ); }
// SCDOM_RESULT  SciterGetElementIntrinsicWidths( HELEMENT he, INT* pMinWidth, INT* pMaxWidth ) ;//{ return SAPI()->SciterGetElementIntrinsicWidths(he,pMinWidth,pMaxWidth ); }
// SCDOM_RESULT  SciterGetElementIntrinsicHeight( HELEMENT he, INT forWidth, INT* pHeight ) ;//{ return SAPI()->SciterGetElementIntrinsicHeight( he,forWidth,pHeight ); }

// SCDOM_RESULT  SciterIsElementVisible( HELEMENT he, BOOL* pVisible)

// SciterIsElementVisible - deep visibility, determines if element visible - has no visiblity:hidden and no display:none defined
//     for itself or for any its parents.
//  \param[in] he \b HELEMENT, element.
//  \param[out] pVisible \b LPBOOL, visibility state.
func (e *Element) IsVisible() bool {
	// args
	var v C.SBOOL
	// cgo call
	C.SciterIsElementVisible(e.handle, &v)
	if v == C.TRUE {
		return true
	}
	return false
}

// SCDOM_RESULT  SciterIsElementEnabled( HELEMENT he, BOOL* pEnabled )

// SciterIsElementEnabled - deep enable state, determines if element enabled - is not disabled by itself or no one
//    of its parents is disabled.
// \param[in] he \b HELEMENT, element.
// \param[out] pEnabled \b LPBOOL, enabled state.
func (e *Element) IsEnabled() bool {
	// args
	var v C.SBOOL
	// cgo call
	C.SciterIsElementEnabled(e.handle, &v)
	if v == C.TRUE {
		return true
	}
	return false
}

//export goELEMENT_COMPARATOR
func goELEMENT_COMPARATOR(he1 unsafe.Pointer, he2 unsafe.Pointer, arg unsafe.Pointer) int {
	cmp := *(*func(*Element, *Element) int)(arg)
	return cmp(WrapElement(C.HELEMENT(he1)), WrapElement(C.HELEMENT(he2)))
}

// SCDOM_RESULT  SciterSortElements( HELEMENT he, UINT firstIndex, UINT lastIndex, ELEMENT_COMPARATOR* cmpFunc, LPVOID cmpFuncParam ) ;//{ return SAPI()->SciterSortElements( he, firstIndex, lastIndex, cmpFunc, cmpFuncParam ); }

/** SciterSortElements - sort children of the element.
 * \param[in] he \b HELEMENT, element which children to be sorted.
 * \param[in] firstIndex \b UINT, first child index to start sorting from.
 * \param[in] lastIndex \b UINT, last index of the sorting range, element with this index will not be included in the sorting.
 * \param[in] cmpFunc \b ELEMENT_COMPARATOR, comparator function.
 * \param[in] cmpFuncParam \b LPVOID, parameter to be passed in comparator function.
 **/
func (e *Element) Sort(start, count int, comparator func(*Element, *Element) int) error {
	// args
	cstart := C.UINT(start)
	cend := C.UINT(start + count)
	cmpFunc := (*C.ELEMENT_COMPARATOR)(unsafe.Pointer(C.ELEMENT_COMPARATOR_cgo))
	cmpFuncParam := C.LPVOID(unsafe.Pointer(&comparator))
	// cgo call
	r := C.SciterSortElements(e.handle, cstart, cend, cmpFunc, cmpFuncParam)
	return wrapDomResult(r, "SciterSortElements")
}

func (e *Element) SortChildren(comparator func(*Element, *Element) int) error {
	count, err := e.ChildrenCount()
	if err != nil {
		return err
	}
	return e.Sort(0, count, comparator)
}

// SCDOM_RESULT  SciterSwapElements( HELEMENT he1, HELEMENT he2 ) ;//{ return SAPI()->SciterSwapElements( he1,he2 ); }

// SciterSwapElements - swap element positions.
//  Function changes "insertion points" of two elements. So it swops indexes and parents of two elements.
//  \param[in] he1 \b HELEMENT, first element.
//  \param[in] he2 \b HELEMENT, second element.
func (e *Element) Swap(t *Element) error {
	return wrapDomResult(C.SciterSwapElements(e.handle, t.handle), "SciterSwapElements")
}

// SCDOM_RESULT  SciterTraverseUIEvent( UINT evt, LPVOID eventCtlStruct, BOOL* bOutProcessed ) ;//{ return SAPI()->SciterTraverseUIEvent( evt,eventCtlStruct,bOutProcessed ); }

// SCDOM_RESULT  SciterCallScriptingFunction( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) ;//{ return SAPI()->SciterCallScriptingFunction( he,name,argv,argc,retval ); }

// call scripting function defined on global level
// Example, script:
//   function foo() {...}
// Native code:
//   dom::element root = ... get root element of main document or some frame inside it
//   root.call_function("foo"); // call the function
func (e *Element) CallFunction(functionName string, args ...*Value) (retval *Value, err error) {
	retval = NewValue()
	argc := len(args)
	argv := make([]Value, argc)
	for i := 0; i < argc; i++ {
		argv[i].Copy(args[i]) // Make copy that is not garbage collected
		defer argv[i].finalize()
	}
	// args
	cfn := C.LPCSTR(unsafe.Pointer(StringToBytePtr(functionName)))
	cargc := C.UINT(argc)
	var cargv *C.SCITER_VALUE
	if len(argv) > 0 {
		cargv = (*C.SCITER_VALUE)(unsafe.Pointer(&argv[0]))
	}
	cretval := (*C.SCITER_VALUE)(unsafe.Pointer(retval))
	// cgo call
	r := C.SciterCallScriptingFunction(e.handle, cfn, cargv, cargc, cretval)
	err = wrapDomResult(r, "SciterCallScriptingFunction")
	return
}

// SCDOM_RESULT  SciterCallScriptingMethod( HELEMENT he, LPCSTR name, const VALUE* argv, UINT argc, VALUE* retval ) ;//{ return SAPI()->SciterCallScriptingMethod( he,name,argv,argc,retval ); }

// call scripting method attached to the element (directly or through of scripting behavior)
// Example, script:
//   var elem = ...
//   elem.foo = function() {...}
// Native code:
//   dom::element elem = ...
//   elem.call_method("foo");
func (e *Element) CallMethod(methodName string, args ...*Value) (retval *Value, err error) {
	retval = NewValue()
	argc := len(args)
	argv := make([]Value, argc)
	for i := 0; i < argc; i++ {
		argv[i].Copy(args[i]) // Make copy that is not garbage collected
		defer argv[i].finalize()
	}
	// args
	cfn := C.LPCSTR(unsafe.Pointer(StringToBytePtr(methodName)))
	cargc := C.UINT(argc)
	var cargv *C.SCITER_VALUE
	if len(argv) > 0 {
		cargv = (*C.SCITER_VALUE)(unsafe.Pointer(&argv[0]))
	}

	cretval := (*C.SCITER_VALUE)(unsafe.Pointer(retval))
	// cgo call
	r := C.SciterCallScriptingMethod(e.handle, cfn, cargv, cargc, cretval)
	err = wrapDomResult(r, "SciterCallScriptingMethod")
	return
}

// SCDOM_RESULT  SciterEvalElementScript( HELEMENT he, LPCWSTR script, UINT scriptLength, VALUE* retval ) ;//{ return SAPI()->SciterEvalElementScript( he, script, scriptLength, retval ); }
// SCDOM_RESULT  SciterAttachHwndToElement(HELEMENT he, HWINDOW hwnd) ;//{ return SAPI()->SciterAttachHwndToElement(he,hwnd); }
// SCDOM_RESULT  SciterControlGetType( HELEMENT he, /*CTL_TYPE*/ UINT *pType ) ;//{ return SAPI()->SciterControlGetType( he, pType ); }

// SCDOM_RESULT  SciterGetValue( HELEMENT he, VALUE* pval ) ;//{ return SAPI()->SciterGetValue( he,pval ); }

// SciterGetValue - get value of the element. 'value' is value of correspondent behavior attached to the element or its text.
//  \param[in] he \b HELEMENT, element which value will be retrieved.
//  \param[out] pval \b VALUE*, pointer to VALUE that will get elements value.
//   ATTN: if you are not using json::value wrapper then you shall call ValueClear aginst the returned value
//         otherwise memory will leak.
func (e *Element) GetValue() (*Value, error) {
	// args
	v := NewValue()
	cv := (*C.VALUE)(unsafe.Pointer(v))
	// cgo call
	r := C.SciterGetValue(e.handle, cv)
	return v, wrapDomResult(r, "SciterGetValue")
}

// SCDOM_RESULT  SciterSetValue( HELEMENT he, const VALUE* pval ) ;//{ return SAPI()->SciterSetValue( he, pval ); }

// SciterSetValue - set value of the element.
//  \param[in] he \b HELEMENT, element which value will be changed.
//  \param[in] pval \b VALUE*, pointer to the VALUE to set.
func (e *Element) SetValue(v *Value) error {
	// args
	cv := (*C.VALUE)(unsafe.Pointer(v))
	// cgo call
	r := C.SciterSetValue(e.handle, cv)
	return wrapDomResult(r, "SciterSetValue")
}

// SCDOM_RESULT  SciterGetExpando( HELEMENT he, VALUE* pval, BOOL forceCreation ) ;//{ return SAPI()->SciterGetExpando( he, pval, forceCreation ); }
// DLLEXPORT SCDOM_RESULT  SciterGetObject( HELEMENT he, tiscript_value* pval, BOOL forceCreation ) ;//{ return SAPI()->SciterGetObject( he, pval, forceCreation ); }
// DLLEXPORT SCDOM_RESULT  SciterGetElementNamespace(  HELEMENT he, tiscript_value* pval) ;//{ return SAPI()->SciterGetElementNamespace( he,pval); }
// SCDOM_RESULT  SciterGetHighlightedElement(HWINDOW hwnd, HELEMENT* phe) ;//{ return SAPI()->SciterGetHighlightedElement(hwnd, phe); }
// SCDOM_RESULT  SciterSetHighlightedElement(HWINDOW hwnd, HELEMENT he) ;//{ return SAPI()->SciterSetHighlightedElement(hwnd,he); }
// SCDOM_RESULT  SciterNodeAddRef(HNODE hn) ;//{ return SAPI()->SciterNodeAddRef(hn); }
// SCDOM_RESULT  SciterNodeRelease(HNODE hn) ;//{ return SAPI()->SciterNodeRelease(hn); }
// SCDOM_RESULT  SciterNodeCastFromElement(HELEMENT he, HNODE* phn) ;//{ return SAPI()->SciterNodeCastFromElement(he,phn); }
// SCDOM_RESULT  SciterNodeCastToElement(HNODE hn, HELEMENT* he) ;//{ return SAPI()->SciterNodeCastToElement(hn,he); }
// SCDOM_RESULT  SciterNodeFirstChild(HNODE hn, HNODE* phn) ;//{ return SAPI()->SciterNodeFirstChild(hn,phn); }
// SCDOM_RESULT  SciterNodeLastChild(HNODE hn, HNODE* phn) ;//{ return SAPI()->SciterNodeLastChild(hn, phn); }
// SCDOM_RESULT  SciterNodeNextSibling(HNODE hn, HNODE* phn) ;//{ return SAPI()->SciterNodeNextSibling(hn, phn); }
// SCDOM_RESULT  SciterNodePrevSibling(HNODE hn, HNODE* phn) ;//{ return SAPI()->SciterNodePrevSibling(hn,phn); }
// SCDOM_RESULT  SciterNodeParent(HNODE hnode, HELEMENT* pheParent) ;//{ return SAPI()->SciterNodeParent(hnode,pheParent) ; }
// SCDOM_RESULT  SciterNodeNthChild(HNODE hnode, UINT n, HNODE* phn) ;//{ return SAPI()->SciterNodeNthChild(hnode,n,phn); }
// SCDOM_RESULT  SciterNodeChildrenCount(HNODE hnode, UINT* pn) ;//{ return SAPI()->SciterNodeChildrenCount(hnode, pn); }
// SCDOM_RESULT  SciterNodeType(HNODE hnode, UINT* pNodeType /*NODE_TYPE*/) ;//{ return SAPI()->SciterNodeType(hnode,pNodeType); }
// SCDOM_RESULT  SciterNodeGetText(HNODE hnode, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param) ;//{ return SAPI()->SciterNodeGetText(hnode,rcv,rcv_param); }
// SCDOM_RESULT  SciterNodeSetText(HNODE hnode, LPCWSTR text, UINT textLength) ;//{ return SAPI()->SciterNodeSetText(hnode,text,textLength); }
// SCDOM_RESULT  SciterNodeInsert(HNODE hnode, UINT where /*NODE_INS_TARGET*/, HNODE what) ;//{ return SAPI()->SciterNodeInsert(hnode,where,what); }
// SCDOM_RESULT  SciterNodeRemove(HNODE hnode, BOOL finalize) ;//{ return SAPI()->SciterNodeRemove(hnode,finalize); }
// SCDOM_RESULT  SciterCreateTextNode(LPCWSTR text, UINT textLength, HNODE* phnode) ;//{ return SAPI()->SciterCreateTextNode(text,textLength,phnode); }
// SCDOM_RESULT  SciterCreateCommentNode(LPCWSTR text, UINT textLength, HNODE* phnode) ;//{ return SAPI()->SciterCreateCommentNode(text,textLength,phnode); }

// DLLEXPORT HVM    SciterGetVM( HWINDOW hwnd )  ;//{ return SAPI()->SciterGetVM(hwnd); }

// typedef struct
// {
//   UINT   t;
//   UINT   u;
//   UINT64 d;
// } VALUE;
type Value C.VALUE

// type Value struct {
// 	t, u uint
// 	d    uint64
// }

// UINT  ValueInit ( VALUE* pval ) ;//{ return SAPI()->ValueInit(pval); }

// ValueInit - initialize VALUE storage
//
// This call has to be made before passing VALUE* to any other functions
func (pdst *Value) init() error {
	return wrapValueResult(VALUE_RESULT(C.ValueInit((*C.VALUE)(unsafe.Pointer(pdst)))), "ValueInit")
}

// UINT  ValueClear ( VALUE* pval ) ;//{ return SAPI()->ValueClear(pval); }

// ValueClear - clears the VALUE and deallocates all assosiated structures that are not used anywhere else.
func (pdst *Value) clear() error {
	return wrapValueResult(VALUE_RESULT(C.ValueClear((*C.VALUE)(unsafe.Pointer(pdst)))), "ValueClear")
}

// UINT  ValueCompare ( const VALUE* pval1, const VALUE* pval2 ) ;//{ return SAPI()->ValueCompare(pval1,pval2); }

//  ValueCompare - compares two values, returns HV_OK_TRUE if val1 == val2.
func (pdst *Value) Compare(val *Value) error {
	pv := (*C.VALUE)(unsafe.Pointer(pdst))
	v := (*C.VALUE)(unsafe.Pointer(val))
	return wrapValueResult(VALUE_RESULT(C.ValueCompare(pv, v)), "ValueCompare")
}

// UINT  ValueCopy ( VALUE* pdst, const VALUE* psrc ) ;//{ return SAPI()->ValueCopy(pdst, psrc); }

// ValueCopy - copies src VALUE to dst VALUE. dst VALUE must be in ValueInit state.
func (pdst *Value) Copy(psrc *Value) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cpsrc := (*C.VALUE)(unsafe.Pointer(psrc))
	return wrapValueResult(VALUE_RESULT(C.ValueCopy(cpdst, cpsrc)), "ValueCopy")
}

// UINT  ValueIsolate ( VALUE* pdst ) ;//{ return SAPI()->ValueIsolate(pdst); }

// ValueIsolate - converts T_OBJECT value types to T_MAP or T_ARRAY.
// use this method if you need to pass values between different threads.
// The fanction is applicable for the Sciter
func (pdst *Value) Isolate() error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	return wrapValueResult(VALUE_RESULT(C.ValueIsolate(cpdst)), "ValueIsolate")
}

// UINT  ValueType ( const VALUE* pval, UINT* pType, UINT* pUnits ) ;//{ return SAPI()->ValueType(pval,pType,pUnits); }

// ValueType - returns VALUE_TYPE and VALUE_UNIT_TYPE flags of the VALUE
func (pdst *Value) Type(pType, pUints *uint) uint {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	cpType := (*C.UINT)(unsafe.Pointer(pType))
	cpUnits := (*C.UINT)(unsafe.Pointer(pUints))
	// cgo call
	return uint(C.ValueType(cpdst, cpType, cpUnits))
}

// UINT  ValueStringData ( const VALUE* pval, LPCWSTR* pChars, UINT* pNumChars ) ;//{ return SAPI()->ValueStringData(pval,pChars,pNumChars); }

// ValueStringData - returns string data for T_STRING type
// For T_FUNCTION returns name of the fuction.
func (pdst *Value) stringData() string {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	var pChars *uint16
	pNumChars := uint(0)
	cpChars := (*C.LPCWSTR)(unsafe.Pointer(&pChars))
	cpNumChars := (*C.UINT)(unsafe.Pointer(&pNumChars))
	// cgo call
	ret := VALUE_RESULT(C.ValueStringData(cpdst, cpChars, cpNumChars))
	if ret == HV_OK {
		return Utf16ToString(pChars)
	}
	return ""
}

func (v *Value) Clone() *Value {
	return NewValue(v)
}

func (v *Value) String() string {
	if v.IsString() {
		return v.stringData()
	}
	if v.IsNativeFunctor() || v.IsFunction() {
		return "<functor>"
	}
	t := v.Clone()
	t.ConvertToString(CVT_SIMPLE)
	return t.stringData()
}

// UINT  ValueStringDataSet ( VALUE* pval, LPCWSTR chars, UINT numChars, UINT units ) ;//{ return SAPI()->ValueStringDataSet(pval, chars, numChars, units); }

// ValueStringDataSet - sets VALUE to T_STRING type and copies chars/numChars to
// internal refcounted buffer assosiated with the value.
func (pdst *Value) SetString(str string, uintType ...int) error {
	sType := C.UINT(C.UT_STRING_STRING)
	if len(uintType) > 0 {
		sType = C.UINT(uintType[0])
	}
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	pStr, length := StringToUTF16PtrWithLen(str)
	chars := (C.LPCWSTR)(unsafe.Pointer(pStr))
	numChars := (C.UINT)(length)
	// cgo call
	return wrapValueResult(VALUE_RESULT(C.ValueStringDataSet(cpdst, chars, numChars, sType)), "ValueStringDataSet")
}

func NewSymbol(sym string) *Value {
	v := new(Value)
	v.init()
	runtime.SetFinalizer(v, (*Value).finalize)
	if err := v.SetString(sym, int(C.UT_STRING_SYMBOL)); err != nil {
		log.Panic(err)
	}
	return v
}

// UINT  ValueIntData ( const VALUE* pval, INT* pData ) ;//{ return SAPI()->ValueIntData ( pval, pData ); }
func (pdst *Value) Int() int {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	var data int = 0
	pData := (*C.INT)(unsafe.Pointer(&data))
	ret := VALUE_RESULT(C.ValueIntData(cpdst, pData))
	if ret == HV_OK {
		return data
	}
	return 0
}

// UINT  ValueIntDataSet ( VALUE* pval, INT data, UINT type, UINT units ) ;//{ return SAPI()->ValueIntDataSet ( pval, data,type,units ); }
func (pdst *Value) SetInt(data int) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	return wrapValueResult(VALUE_RESULT(C.ValueIntDataSet(cpdst, C.INT(data), T_INT, 0)), "ValueIntDataSet")
}

func (v *Value) Bool() bool {
	if v.Int() == 0 {
		return false
	}
	return true
}

func (pdst *Value) SetBool(b bool) error {
	data := 0
	if b {
		data = 1
	}
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	return wrapValueResult(VALUE_RESULT(C.ValueIntDataSet(cpdst, C.INT(data), T_BOOL, 0)), "ValueIntDataSet")
}

// UINT  ValueInt64Data ( const VALUE* pval, INT64* pData ) ;//{ return SAPI()->ValueInt64Data ( pval,pData ); }
func (pdst *Value) Int64() int64 {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	var data int64 = 0
	pData := (*C.INT64)(unsafe.Pointer(&data))
	ret := VALUE_RESULT(C.ValueInt64Data(cpdst, pData))
	if ret == HV_OK {
		return data
	}
	return 0
}

// UINT  ValueInt64DataSet ( VALUE* pval, INT64 data, UINT type, UINT units ) ;//{ return SAPI()->ValueInt64DataSet ( pval,data,type,units ); }
func (pdst *Value) SetInt64(data int64, typ uint) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	return wrapValueResult(VALUE_RESULT(C.ValueInt64DataSet(cpdst, C.INT64(data), C.UINT(typ), 0)), "ValueInt64DataSet")
}

// UINT  ValueFloatData ( const VALUE* pval, FLOAT_VALUE* pData ) ;//{ return SAPI()->ValueFloatData ( pval,pData ); }
func (pdst *Value) Float() float64 {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	var data float64 = 0
	pData := (*C.FLOAT_VALUE)(unsafe.Pointer(&data))
	r := C.ValueFloatData(cpdst, pData)
	if r != C.UINT(HV_OK) {
		return 0
	}
	return data
}

// UINT  ValueFloatDataSet ( VALUE* pval, FLOAT_VALUE data, UINT type, UINT units ) ;//{ return SAPI()->ValueFloatDataSet ( pval,data,type,units ); }
func (pdst *Value) SetFloat(data float64) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	return wrapValueResult(VALUE_RESULT(C.ValueFloatDataSet(cpdst, C.FLOAT_VALUE(data), C.UINT(T_FLOAT), 0)), "ValueFloatDataSet")
}

// UINT  ValueBinaryData ( const VALUE* pval, LPCBYTE* pBytes, UINT* pnBytes ) ;//{ return SAPI()->ValueBinaryData ( pval,pBytes,pnBytes ); }

// ValueBinaryData - retreive integer data of T_BYTES type
func (pdst *Value) Bytes() []byte {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	var pv C.LPCBYTE
	var length C.UINT
	pBytes := (*C.LPCBYTE)(unsafe.Pointer(&pv))
	// cgo call
	r := C.ValueBinaryData(cpdst, pBytes, &length)
	if r != C.UINT(HV_OK) {
		return nil
	}
	ret := ByteCPtrToBytes(pv, length)
	return ret
}

// UINT  ValueBinaryDataSet ( VALUE* pval, LPCBYTE pBytes, UINT nBytes, UINT type, UINT units )
func (pdst *Value) SetBytes(data []byte) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	var pBytes C.LPCBYTE
	if len(data) > 0 {
		pBytes = (C.LPCBYTE)(unsafe.Pointer(&data[0]))
	}
	nBytes := (C.UINT)(len(data))
	// cgo call
	r := C.ValueBinaryDataSet(cpdst, pBytes, nBytes, T_BYTES, 0)
	return wrapValueResult(VALUE_RESULT(r), "ValueBinaryDataSet")
}

// UINT  ValueElementsCount ( const VALUE* pval, INT* pn) ;//{ return SAPI()->ValueElementsCount ( pval,pn); }

// ValueElementsCount - retreive number of sub-elements for:
// - T_ARRAY - number of elements in the array;
// - T_MAP - number of key/value pairs in the map;
// - T_FUNCTION - number of arguments in the function;
func (pdst *Value) Length() int {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	//args
	var n C.INT
	r := C.ValueElementsCount(cpdst, &n)
	if r != C.UINT(HV_OK) {
		// in case of not being array/map etc.
		return 0
	}
	return int(n)
}

// UINT  ValueNthElementValue ( const VALUE* pval, INT n, VALUE* pretval) ;//{ return SAPI()->ValueNthElementValue ( pval, n, pretval); }

// ValueNthElementValue - retreive value of sub-element at index n for:
// - T_ARRAY - nth element of the array;
// - T_MAP - value of nth key/value pair in the map;
// - T_FUNCTION - value of nth argument of the function;
func (pdst *Value) Index(n int) *Value {
	ret := NewValue()
	//args
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cn := C.INT(n)
	cret := (*C.VALUE)(unsafe.Pointer(ret))
	// cgo call
	r := C.ValueNthElementValue(cpdst, cn, cret)
	if r != C.UINT(HV_OK) {
		return nil
	}
	return ret
}

// UINT  ValueNthElementValueSet ( VALUE* pval, INT n, const VALUE* pval_to_set) ;//{ return SAPI()->ValueNthElementValueSet ( pval,n,pval_to_set); }

// ValueNthElementValueSet - sets value of sub-element at index n for:
//  - T_ARRAY - nth element of the array;
//  - T_MAP - value of nth key/value pair in the map;
//  - T_FUNCTION - value of nth argument of the function;
// If the VALUE is not of one of types above then it makes it of type T_ARRAY with
// single element - 'val_to_set'.
func (pdst *Value) SetIndex(idx int, val interface{}) error {
	// args
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cval := (*C.VALUE)(unsafe.Pointer(NewValue(val)))
	// cgo call
	return wrapValueResult(VALUE_RESULT(C.ValueNthElementValueSet(cpdst, C.INT(idx), cval)), "ValueNthElementValueSet")
}

func (pdst *Value) Append(val interface{}) error {
	n := pdst.Length()
	return pdst.SetIndex(n, val)
}

// UINT  ValueNthElementKey ( const VALUE* pval, INT n, VALUE* pretval) ;//{ return SAPI()->ValueNthElementKey ( pval,n,pretval); }
func (pdst *Value) NthElementKey(idx int) (val *Value) {
	val = NewValue()
	// args
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cval := (*C.VALUE)(unsafe.Pointer(val))
	// cgo call
	r := C.ValueNthElementKey(cpdst, C.INT(idx), cval)
	// ret
	if r != C.UINT(HV_OK) {
		return nil
	}
	return val
}

/**Callback function used with #ValueEnumElements().
 * return TRUE to continue enumeration
 */
// typedef BOOL SC_CALLBACK KeyValueCallback( LPVOID param, const VALUE* pkey, const VALUE* pval );

type KeyValueCallback func(key, value *Value) bool

//export goKeyValueCallback
func goKeyValueCallback(param unsafe.Pointer, key, value *Value) bool {
	fn := *(*KeyValueCallback)(param)
	return fn(key, value)
}

/**
 * ValueEnumElements - enumeartes key/value pairs of T_MAP, T_FUNCTION and T_OBJECT values
 * - T_MAP - key of nth key/value pair in the map;
 * - T_FUNCTION - name of nth argument of the function (if any);
 */
// UINT  ValueEnumElements ( VALUE* pval, KeyValueCallback* penum, LPVOID param) ;//{ return SAPI()->ValueEnumElements (pval,penum,param); }
func (pdst *Value) EnumerateKeyValue(fn KeyValueCallback) error {
	// args
	cpval := (*C.VALUE)(unsafe.Pointer(pdst))
	cpenum := (*C.KeyValueCallback)(unsafe.Pointer(C.KeyValueCallback_cgo))
	cparam := C.LPVOID(unsafe.Pointer(&fn))
	// cgo call
	return wrapValueResult(VALUE_RESULT(C.ValueEnumElements(cpval, cpenum, cparam)), "ValueEnumElements")
}

// UINT  ValueSetValueToKey ( VALUE* pval, const VALUE* pkey, const VALUE* pval_to_set) ;//{ return SAPI()->ValueSetValueToKey ( pval, pkey, pval_to_set); }

//  ValueSetValueToKey - sets value of sub-element by key:
//
//    - T_MAP - value of key/value pair with the key;
//    - T_FUNCTION - value of argument with the name key;
//    - T_OBJECT (tiscript) - value of property of the object
//  If the VALUE is not of one of types above then it makes it of type T_MAP with
//  single pair - 'key'/'val_to_set'.
//
//  key usually is a value of type T_STRING
func (pdst *Value) Set(key string, val interface{}) error {
	// args
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cval := (*C.VALUE)(unsafe.Pointer(NewValue(val)))
	vkey := NewValue(key)
	ckey := (*C.VALUE)(unsafe.Pointer(vkey))
	// cgo call
	return wrapValueResult(VALUE_RESULT(C.ValueSetValueToKey(cpdst, ckey, cval)), "ValueSetValueToKey")
}

// UINT  ValueGetValueOfKey ( const VALUE* pval, const VALUE* pkey, VALUE* pretval) ;//{ return SAPI()->ValueGetValueOfKey ( pval, pkey,pretval); }

// ValueGetValueOfKey - retrieves value of sub-element by key:
//   - T_MAP - value of key/value pair with the key;
//   - T_FUNCTION - value of argument with the name key;
//   - T_OBJECT (tiscript) - value of property of the object
// Otherwise *pretval will have T_UNDEFINED value.
func (pdst *Value) Get(key string) (val *Value) {
	val = NewValue()
	// args
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	cval := (*C.VALUE)(unsafe.Pointer(val))
	vkey := NewValue(key)
	ckey := (*C.VALUE)(unsafe.Pointer(vkey))
	// cgo call
	r := C.ValueGetValueOfKey(cpdst, ckey, cval)
	// ret
	if r != C.UINT(HV_OK) {
		return nil
	}
	return val
}

// UINT  ValueToString ( VALUE* pval, UINT how ) ;//{ return SAPI()->ValueToString ( pval,how ); }

// ValueToString - converts value to T_STRING inplace:
//   - CVT_SIMPLE - parse/emit terminal values (T_INT, T_FLOAT, T_LENGTH, T_STRING)
//   - CVT_JSON_LITERAL - parse/emit value using JSON literal rules: {}, [], "string", true, false, null
//   - CVT_JSON_MAP - parse/emit MAP value without enclosing '{' and '}' brackets.
func (pdst *Value) ConvertToString(how ValueStringConvertType) error {
	cpdst := (*C.VALUE)(unsafe.Pointer(pdst))
	//args
	chow := C.UINT(how)
	return wrapValueResult(VALUE_RESULT(C.ValueToString(cpdst, chow)), "ValueToString")
}

// UINT  ValueFromString ( VALUE* pval, LPCWSTR str, UINT strLength, UINT how ) ;//{ return SAPI()->ValueFromString ( pval, str,strLength,how ); }

// ValueFromString - parses string into value:
//  - CVT_SIMPLE - parse/emit terminal values (T_INT, T_FLOAT, T_LENGTH, T_STRING), "guess" non-strict parsing
//  - CVT_JSON_LITERAL - parse/emit value using JSON literal rules: {}, [], "string", true, false, null
//  - CVT_JSON_MAP - parse/emit MAP value without enclosing '{' and '}' brackets.
// Returns:
//   Number of non-parsed characters in case of errors. Thus if string was parsed in full it returns 0 (success)
func (pdst *Value) ConvertFromString(str string, how ValueStringConvertType) error {
	cval := (*C.VALUE)(unsafe.Pointer(pdst))
	// args
	u16, err := Utf16FromString(str)
	if err != nil {
		return err
	}
	clen := C.UINT(len(u16) - 1)
	cstr := C.LPCWSTR(unsafe.Pointer(&u16[0]))
	chow := C.UINT(how)
	// cgo call
	return wrapValueResult(VALUE_RESULT(C.ValueFromString(cval, cstr, clen, chow)), "ValueFromString")
}

// UINT  ValueInvoke ( VALUE* pval, VALUE* pthis, UINT argc, const VALUE* argv, VALUE* pretval, LPCWSTR url) ;//{ return SAPI()->ValueInvoke ( pval, pthis, argc, argv, pretval, url); }

//ValueInvoke - function invocation (Sciter/TIScript). Usually as callbacks for asynchronous tasks
//  - VALUE* pval is a value of type T_OBJECT/UT_OBJECT_FUNCTION
//  - VALUE* self - object that will be known as 'this' inside that function.
//                   'self' here is what will be known as 'this' inside the function,
//                   can be undefined for invocations of global functions
//  - UINT argc, const VALUE* argv - vector of arguments to pass to the function.
//  - VALUE* pretval - parse/emit MAP value without enclosing '{' and '}' brackets.
//  - LPCWSTR url - url or name of the script - used for error reporting in the script.
//Returns:
//  HV_OK, HV_BAD_PARAMETER or HV_INCOMPATIBLE_TYPE
func (v *Value) Invoke(self *Value, nameOrUrl string, args ...*Value) (retval *Value, err error) {
	retval = NewValue()
	argc := len(args)
	argv := make([]Value, argc)
	for i := 0; i < argc; i++ {
		argv[i].Copy(args[i]) // Make copy that is not garbage collected
		defer argv[i].finalize()
	}
	// args
	cv := (*C.SCITER_VALUE)(unsafe.Pointer(v))
	cself := (*C.SCITER_VALUE)(unsafe.Pointer(self))
	cargc := C.UINT(argc)
	var cargv *C.SCITER_VALUE
	if len(argv) > 0 {
		cargv = (*C.SCITER_VALUE)(unsafe.Pointer(&argv[0]))
	}
	cretval := (*C.SCITER_VALUE)(unsafe.Pointer(retval))
	cnameOrUrl := C.LPCWSTR(StringToWcharPtr(nameOrUrl))
	// cgo call
	r := C.ValueInvoke(cv, cself, cargc, cargv, cretval, cnameOrUrl)
	err = wrapValueResult(VALUE_RESULT(r), "ValueInvoke")
	return
}

var (
	functorMap = make(map[unsafe.Pointer]NativeFunctor, 20)
)

// Native functor
// typedef VOID NATIVE_FUNCTOR_INVOKE( VOID* tag, UINT argc, const VALUE* argv, VALUE* retval); // retval may contain error definition

//export goNATIVE_FUNCTOR_INVOKE
func goNATIVE_FUNCTOR_INVOKE(tag unsafe.Pointer, argc uint, argv unsafe.Pointer, retval unsafe.Pointer) uint {
	nf := functorMap[tag]
	args := make([]*Value, int(argc))
	step := unsafe.Sizeof(Value{})
	rval := (*Value)(retval)
	p := uintptr(argv)
	for i := 0; i < int(argc); i++ {
		args[i] = (*Value)(unsafe.Pointer(p + uintptr(i)*step))
	}
	val := nf(args...)
	rval.init()
	rval.Copy(val)
	return 1
}

//export goNATIVE_FUNCTOR_RELEASE
func goNATIVE_FUNCTOR_RELEASE(tag unsafe.Pointer) int {
	delete(functorMap, tag)
	return 1
}

var (
	pinvoke  = (*C.NATIVE_FUNCTOR_INVOKE)(unsafe.Pointer(C.NATIVE_FUNCTOR_INVOKE_cgo))
	prelease = (*C.NATIVE_FUNCTOR_RELEASE)(unsafe.Pointer(C.NATIVE_FUNCTOR_RELEASE_cgo))
)

// typedef VOID NATIVE_FUNCTOR_RELEASE( VOID* tag );

// UINT  ValueNativeFunctorSet (VALUE* pval, NATIVE_FUNCTOR_INVOKE*  pinvoke, NATIVE_FUNCTOR_RELEASE* prelease, VOID* tag )

//ValueNativeFunctorSet - set reference to native function
//  - VALUE* pval - value to be initialized
//  - NATIVE_FUNCTOR_INVOKE* pinvoke - reference to native functor implementation.
//  - NATIVE_FUNCTOR_RELEASE* prelease - reference to native functor dtor implementation.
//  - VOID* tag - optional tag, passed as it is to pinvoke and prelease
//Returns:
//  HV_OK, HV_BAD_PARAMETER
func (pdst *Value) SetNativeFunctor(nf NativeFunctor) error {
	tag := unsafe.Pointer(&nf)
	functorMap[tag] = nf
	// args
	cval := (*C.VALUE)(unsafe.Pointer(pdst))
	// cgo call
	r := C.ValueNativeFunctorSet(cval, pinvoke, prelease, tag)
	return wrapValueResult(VALUE_RESULT(r), "ValueNativeFunctorSet")
}

// BOOL  ValueIsNativeFunctor ( const VALUE* pval) ;//{ return SAPI()->ValueIsNativeFunctor (pval); }
func (pdst *Value) IsNativeFunctor() bool {
	cval := (*C.VALUE)(unsafe.Pointer(pdst))
	r := C.ValueIsNativeFunctor(cval)
	if r == 0 {
		return false
	}
	return true
}
