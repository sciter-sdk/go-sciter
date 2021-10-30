package resx

import (
	"embed"
	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/resx"
	"github.com/sciter-sdk/go-sciter/window"
	"log"
)


// embed resource directory

//go:embed html/*
var f embed.FS

func main() {
	win, err := window.New(sciter.DefaultWindowCreateFlag, nil)
	if err != nil {
		log.Fatal(err)
	}
	sciter.SetOption(sciter.SCITER_SET_DEBUG_MODE, 1)
	//use embed resource
	resx.HandleDataLoad(win.Sciter, f)
	//load embed file
	win.LoadFile("resx://html/index.html")
	win.SetTitle("title")
	win.Show()
	win.Run()
	log.Println(".....exit.....")
}
