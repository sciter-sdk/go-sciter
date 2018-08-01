package main

import (
	"log"
	tool "github.com/GeertJohan/go.rice"
	sciter "github.com/sciter-sdk/go-sciter"
	rice "github.com/sciter-sdk/go-sciter/rice"
	window "github.com/sciter-sdk/go-sciter/window"
)

/*
A go.rice usage example.

To build it you need an actual `go.rice` package and its tool:

* install go.gice: `go get github.com/GeertJohan/go.rice && go get github.com/GeertJohan/go.rice/rice`
* append resources to executable via `go build && rice append /exec restest.exe` (omit .exe if not on Windows)
* or you can embed it via `rice embed-go && go build` (in that order)

Now the resulting executable is completely stand-alone.
*/
func main() {
	// As usual, create a sciter window.
	w, err := window.New(sciter.SW_TITLEBAR|sciter.SW_RESIZEABLE|sciter.SW_CONTROLS|sciter.SW_MAIN, nil)
	if err != nil {
		log.Fatal(err)
	}

	// 1. Handle resources via sciter.rice loader.
	// It handles URLs like `file://box/` and `rice://box/`.
	rice.HandleDataLoad(w.Sciter)

	// 2. A dummy call to allow `go.rice` to package and register that folder.
	tool.MustFindBox("res")

	// 3. Load a packaged resource.
	err = w.LoadFile("rice://res/simple.html")
	if err != nil {
		log.Fatal(err)
	}
	w.Show()
	w.Run()
}
