package main

import "C"
import (
	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	win, _ := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	win.AddQuitMenu()
	win.Show()
	win.Run()
}
