package window

/*
#include <gtk/gtk.h>
#include <stdio.h>
#cgo linux pkg-config: gtk+-3.0
GtkWindow* gwindow(GtkWidget* hwnd) {
	printf("hwnd:%d\n", hwnd);
	return GTK_WINDOW(gtk_widget_get_toplevel(hwnd));
}
GtkWindow* gshow(GtkWidget* hwnd){
	GtkWidget* gw = gtk_widget_get_toplevel(hwnd);
	// gtk_widget_show_all(gw);
	gtk_window_present(GTK_WINDOW(gw));
}
*/
import "C"
import (
	"fmt"
	"github.com/oskca/sciter"
	"unsafe"
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
