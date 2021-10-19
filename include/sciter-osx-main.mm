//
//  sciter-osx-main.mm
//  sciter
//
//  Created by andrew on 2014-03-23.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>

#include "sciter-x-window.hpp"


static std::vector<sciter::string> _argv;

#ifndef SKIP_MAIN

int main(int argc, const char * argv[])
{
  NSApplication * application = [NSApplication sharedApplication];
  NSArray *tl;
  auto mb = [NSBundle mainBundle];
  [mb loadNibNamed:@"MainMenu" owner:application topLevelObjects:&tl];
  
    
  for( int i = 0; i < argc; ++i ) {
      aux::a2w w(argv[i]);
      _argv.push_back(sciter::string(w.c_str(),w.length()));
  }
    
  auto message_pump = [&]() -> int {
    [application run];
    return 0;
  };
    
  return uimain(message_pump);
  
  /*auto message_pump = [&]() -> int {
    return NSApplicationMain(argc,argv);
  };
  return uimain(message_pump);*/
}
#endif

namespace sciter {
    
  NSView*   nsview(HWINDOW hwnd) { return (__bridge NSView*) hwnd; }
  NSWindow* nswindow(HWINDOW hwnd) { return hwnd ? [nsview(hwnd) window]:nullptr; }
   
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
    if(_hwnd) [nswindow(_hwnd) performMiniaturize:nsview(_hwnd)];
  }
  void window::expand( bool maximize) { 
    if(_hwnd) [nswindow(_hwnd) makeKeyAndOrderFront:nil];
  }    

  void window::dismiss() {
    if(_hwnd) [nswindow(_hwnd) performClose: nswindow(_hwnd)];
    _hwnd = 0;
  }

  window::window( UINT creationFlags, RECT frame): _hwnd(NULL)
  {
     asset_add_ref();
    _hwnd = ::SciterCreateWindow(creationFlags, (frame.right - frame.left) > 0 ? &frame: NULL,NULL,this,NULL);
  }

}
