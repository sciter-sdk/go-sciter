package main

import (
	"flag"
	"log"
	"path/filepath"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		log.Fatal("html file needed")
	}
	rect := sciter.NewRect(300, 300, 300, 400)
	// create window
	w, err := window.New(sciter.DefaultWindowCreateFlag, rect)
	if err != nil {
		log.Fatal(err)
	}
	fullpath, err := filepath.Abs(flag.Arg(0)) //if it's already an absolute path, it'll be returned as is.
	if err != nil {
		log.Fatal(err)
	}
	w.LoadFile(fullpath)
	w.Show()
	w.Run()
}
