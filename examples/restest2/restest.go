package main

import (
	"log"
	sciter "github.com/sciter-sdk/go-sciter"
	window "github.com/sciter-sdk/go-sciter/window"
)

/*
A Sciter archive usage example.

To build it you need the `packfolder` tool from the Sciter SDK:

* convert resources from folder `res` to `res.go` via `packfolder res res.go -v resource_name -go`
* use it in your source code via `win.SetResourceArchive(resource_name)`

Now the resulting executable is completely stand-alone.
*/
func main() {
	// As usual, create a sciter window.
	w, err := window.New(sciter.SW_TITLEBAR|sciter.SW_RESIZEABLE|sciter.SW_CONTROLS|sciter.SW_MAIN, nil)
	if err != nil {
		log.Fatal(err)
	}

	// 1. Handle resources via sciter archive loader.
	// It handles URLs like `this://app/`.
	w.SetResourceArchive(resource_name)

	// 2. Load a packaged resource.
	err = w.LoadFile("this://app/simple.html")
	if err != nil {
		log.Fatal(err)
	}
	w.Show()
	w.Run()
}
