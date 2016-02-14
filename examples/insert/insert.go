package main

import (
	"github.com/oskca/sciter"
	"github.com/oskca/sciter/window"
	"log"
)

func main() {
	w, err := window.New(sciter.SW_TITLEBAR|sciter.SW_RESIZEABLE|sciter.SW_CONTROLS|sciter.SW_MAIN|sciter.SW_ENABLE_DEBUG, nil)
	if err != nil {
		log.Fatal(err)
	}
	// log.Printf("handle: %v", w.Handle)
	// w.LoadFile("simple.html")
	w.LoadHtml("<html>asdf</html>", "")
	w.SetTitle("Inserting Example")
	root, err := w.GetRootElement()
	if err != nil {
		log.Panicln(err)
	}
	div, err := sciter.CreateElement("div", "hello world")
	if err != nil {
		log.Printf("sciter.CreateElement('div', '') %s", err.Error())
		return
	}
	err = root.Insert(div, 0)
	if err != nil {
		log.Printf("root.Insert(div, 0) %s", err.Error())
		return
	}
	w.Show()
	w.Run()
}
