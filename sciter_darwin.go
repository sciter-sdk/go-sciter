package sciter

/*
#cgo CFLAGS: -Iinclude
#include "sciter-x.h"
*/
import "C"
import (
	"unsafe"
)

// HWINDOW  SciterCreateWindow ( UINT creationFlags,LPRECT frame, SciterWindowDelegate* delegate, LPVOID delegateParam, HWINDOW parent);

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
		nil,
		(C.LPVOID)(delegateParam),
		parent)
	// in case of NULL
	if int(uintptr(unsafe.Pointer(hwnd))) == 0 {
		return BAD_HWINDOW
	}
	return hwnd
}
