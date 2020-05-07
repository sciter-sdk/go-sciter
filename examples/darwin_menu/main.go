package main

/*
#cgo CFLAGS: -x objective-c
#cgo LDFLAGS: -framework Cocoa
#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>
#include <stdlib.h>

void MinimalMenu(void) {

    id menubar = [[NSMenu new] autorelease];
    id appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appMenu = [[NSMenu new] autorelease];
	id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"I want to " stringByAppendingString:appName];
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
        action:@selector(terminate:) keyEquivalent:@"q"]
			  autorelease];

	[appMenu addItem:quitMenuItem];
	[appMenuItem setSubmenu:appMenu];
}
*/

import "C"
import (
	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	win, _ := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	win.AddMenu( [menus as parameter ])
	win.Show()
	win.Run()
}
