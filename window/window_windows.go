package window

/*
#include <windows.h>
*/
import "C"
import (
	"fmt"
	"syscall"
	"unsafe"

	"github.com/lxn/win"
	"github.com/sciter-sdk/go-sciter"
)

func New(creationFlags sciter.WindowCreationFlag, rect *sciter.Rect) (*Window, error) {
	w := new(Window)
	w.creationFlags = creationFlags

	// Initialize OLE for DnD and printing support
	win.OleInitialize()

	// create window
	hwnd := sciter.CreateWindow(
		creationFlags,
		rect,
		syscall.NewCallback(delegateProc),
		0,
		sciter.BAD_HWINDOW)

	if hwnd == sciter.BAD_HWINDOW {
		return nil, fmt.Errorf("Sciter CreateWindow failed [%d]", win.GetLastError())
	}

	w.Sciter = sciter.Wrap(hwnd)
	return w, nil
}

func (s *Window) Show() {
	// message handling
	hwnd := win.HWND(unsafe.Pointer(s.GetHwnd()))
	win.ShowWindow(hwnd, win.SW_SHOW)
	win.UpdateWindow(hwnd)
}

func (s *Window) SetTitle(title string) {
	// message handling
	hwnd := C.HWND(unsafe.Pointer(s.GetHwnd()))
	C.SetWindowTextW(hwnd, (*C.WCHAR)(unsafe.Pointer(sciter.StringToWcharPtr(title))))
}

func (s *Window) AddQuitMenu() {
	// Define behaviour for windows
}

func (s *Window) Run() {
	// for system drag-n-drop
	// win.OleInitialize()
	// defer win.OleUninitialize()
	s.run()
	// start main gui message loop
	msg := (*win.MSG)(unsafe.Pointer(win.GlobalAlloc(0, unsafe.Sizeof(win.MSG{}))))
	defer win.GlobalFree(win.HGLOBAL(unsafe.Pointer(msg)))
	for win.GetMessage(msg, 0, 0, 0) > 0 {
		win.TranslateMessage(msg)
		win.DispatchMessage(msg)
	}
}

// delegate Windows GUI messsage
func delegateProc(hWnd win.HWND, message uint, wParam uintptr, lParam uintptr, pParam uintptr, pHandled *int) int {
	switch message {
	case win.WM_DESTROY:
		// log.Println("closing window ...")
		win.PostQuitMessage(0)
		*pHandled = 1
	}
	return 0
}
