//
//  sciter-gtk-main.mm
//  sciter
//
//  Created by andrew on 2014-12-05.
//  Copyright (c) 2014 Andrew Fedoniouk. All rights reserved.
//

#include <gtk/gtk.h>
#include <locale.h>

#include "sciter-x-window.hpp"

static std::vector<sciter::string> _argv;

#ifndef SKIP_MAIN
int main (int argc, char *argv[])
{
  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  setlocale(LC_TIME,"");

  for( int i = 0; i < argc; ++i ) {
      aux::a2w w(argv[i]);
      _argv.push_back(sciter::string(w.c_str(),w.length()));
  }

  auto message_pump = []() -> int {
    gtk_main ();
    return 0;
  };

  return uimain(message_pump);
}
#endif


namespace sciter {

  GtkWidget* gview(HWINDOW hwnd) { return hwnd; }
  GtkWindow* gwindow(HWINDOW hwnd) { return hwnd ? GTK_WINDOW(gtk_widget_get_toplevel(hwnd)): nullptr; }

  namespace application {
    HINSTANCE hinstance()
    {
      return nullptr; // not used
    }

    const std::vector<sciter::string>& argv() {
      return _argv;
    }
  }

  void window::collapse() {
    if(_hwnd) gtk_window_iconify (gwindow(_hwnd));
  }
  void window::expand( bool maximize) {
    if(_hwnd) gtk_window_present (gwindow(_hwnd));
  }

  void window::dismiss() {
    if(_hwnd) gtk_window_close (gwindow(_hwnd));
    _hwnd = 0; //?
  }

  window::window( UINT creationFlags, RECT frame): _hwnd(NULL)
  {
    asset_add_ref();
    _hwnd = SAPI()->SciterCreateWindow(creationFlags, (frame.right - frame.left) > 0 ? &frame: NULL,NULL,this,NULL);
  }

}
