package window

/*
#cgo linux pkg-config: gtk+-3.0

#include <gtk/gtk.h>

GtkWindow* gwindow(GtkWidget* hwnd) {
  return hwnd ? GTK_WINDOW(gtk_widget_get_toplevel(hwnd)) : NULL;
}

void gshow(GtkWidget* hwnd){
  if(hwnd) gtk_window_present(gwindow(hwnd));
}

*/
import "C"
import (
	"fmt"
	"unsafe"

	"github.com/sciter-sdk/go-sciter"
)

// Linux/gtk3 must (at least) use sciter.DefaultWindowCreationFlag to create the main window correctly
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
	w := C.gwindow((*C.GtkWidget)(unsafe.Pointer(s.GetHwnd())))
	C.gtk_window_set_title(w, (*C.gchar)(unsafe.Pointer(&([]byte(title))[0])))
}

func (s *Window) Show() {
	w := (*C.GtkWidget)(unsafe.Pointer(s.GetHwnd()))
	C.gshow(w)
}

func (s *Window) Run() {
	s.run()
	C.gtk_main()
}

func init() {
	C.gtk_init(nil, nil)
}
