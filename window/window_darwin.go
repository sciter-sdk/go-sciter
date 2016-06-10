// +build darwin

package window

import (
	"fmt"
	"log"

	"github.com/oskca/sciter"
	// "runtime"
)

// func (w *Window) run() {
// 	// runtime.LockOSThread()
// }

func New(creationFlags sciter.WindowCreationFlag, rect *sciter.Rect) (*Window, error) {
	w := new(Window)
	w.creationFlags = creationFlags
	// create window
	hwnd := sciter.CreateWindow(
		creationFlags,
		rect,
		0,
		0,
		sciter.BAD_HWINDOW)

	if hwnd == sciter.BAD_HWINDOW {
		return nil, fmt.Errorf("Sciter CreateWindow failed")
	}

	w.Sciter = sciter.Wrap(hwnd)
	return w, nil
}
func (s *Window) SetTitle(title string) {
	log.Print("SetTitle in window_darwin.go not implemented")
}

func (s *Window) Show() {
	log.Print("Show in window_darwin.go not implemented")
}

func (s *Window) Run() {
	log.Print("OS X not yet supported in Sciter, so Run isn't doing anything yet.")
	s.run()
	//C.gtk_main()
}
