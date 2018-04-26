package sciter

/*
#cgo CFLAGS: -Iinclude
#include "sciter-x.h"
*/
import "C"
import (
	"github.com/lxn/win"
	"unsafe"
)

// LRESULT  SciterProc (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam) ;//{ return SAPI()->SciterProc (hwnd,msg,wParam,lParam); }
// LRESULT  SciterProcND (HWINDOW hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* pbHandled) ;//{ return SAPI()->SciterProcND (hwnd,msg,wParam,lParam,pbHandled); }

func ProcND(hwnd win.HWND, msg uint, wParam uintptr, lParam uintptr) (ret int, handled bool) {
	var bHandled C.BOOL
	// ret = uintptr(syssciterProcND(HWND(hwnd), msg, wParam, lParam, &bHandled))
	ret = int(C.SciterProcND(C.HWINDOW(unsafe.Pointer(hwnd)), C.UINT(msg), C.WPARAM(wParam), C.LPARAM(lParam), &bHandled))
	if bHandled == 0 {
		handled = false
	} else {
		handled = true
	}
	return
}

// HWINDOW  SciterCreateWindow ( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent);

// Create sciter window.
//   On Windows returns HWND of either top-level or child window created.
//   On OS X returns NSView* of either top-level window or child view .
//
//  \param[in] creationFlags \b SCITER_CREATE_WINDOW_FLAGS, creation flags.
//  \param[in] frame \b LPRECT, window frame position and size.
//  \param[in] delegate \b SciterWindowDelegate, either partial WinProc implementation or thing implementing NSWindowDelegate protocol.
//  \param[in] delegateParam \b LPVOID, optional param passed to SciterWindowDelegate.
//  \param[in] parent \b HWINDOW, optional parent window.
// rect is the display area
func CreateWindow(createFlags WindowCreationFlag, rect *Rect, delegate uintptr, delegateParam uintptr, parent C.HWINDOW) C.HWINDOW {
	// set default size
	if rect == nil {
		rect = DefaultRect
	}
	// create window
	hwnd := C.SciterCreateWindow(
		C.UINT(createFlags),
		(*C.RECT)(unsafe.Pointer(rect)),
		(*C.SciterWindowDelegate)(unsafe.Pointer(delegate)),
		(C.LPVOID)(delegateParam),
		parent)
	// in case of NULL
	if int(uintptr(unsafe.Pointer(hwnd))) == 0 {
		return BAD_HWINDOW
	}
	return hwnd
}
