package main

import (
	"flag"
	"log"
	"path/filepath"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

var (
	pageURI = ""
)

func setupViewer(w *window.Window) {
	log.Println("setup for new view ...")
	w.SetTitle("Sciter HTM viewer")
	root, err := w.GetRootElement()
	if err != nil {
		log.Println(err)
		return
	}
	root.AttachEventHandler(&sciter.EventHandler{
		OnKey: func(el *sciter.Element, p *sciter.KeyParams) bool {
			if p.Cmd == sciter.KEY_DOWN|sciter.SINKING && p.KeyCode == 0x74 {
				log.Println("reloading", pageURI)
				w.LoadFile(pageURI)
				setupViewer(w)
				return true
			}
			return false
		},
		OnExchange: func(el *sciter.Element, p *sciter.ExchangeParams) bool {
			if p.Cmd == sciter.X_WILL_ACCEPT_DROP|sciter.SINKING {
				if p.Data.IsString() {
					return true
				}
			}
			if p.Cmd == sciter.X_DROP|sciter.SINKING {
				pageURI = p.Data.String()
				log.Println("Loading new content:", pageURI)
				w.LoadFile(pageURI)
				setupViewer(w)
				return true
			}
			return true
		},
	})
}

func setCallbackHandlers(w *window.Window) {
	h := &sciter.CallbackHandler{
		OnDataLoaded: func(ld *sciter.ScnDataLoaded) int {
			uri := ld.Uri()
			log.Println("loaded", uri)
			return sciter.LOAD_OK
		},
		OnLoadData: func(ld *sciter.ScnLoadData) int {
			log.Println("loading", ld.Uri())
			return sciter.LOAD_OK
		},
	}
	w.SetCallback(h)
}

func main() {
	flag.Parse()
	// create window
	w, err := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Sciter Version: %X %X\n", sciter.Version(true), sciter.Version(false))
	// enable debug
	ok := w.SetOption(sciter.SCITER_SET_DEBUG_MODE, 1)
	if !ok {
		log.Println("set debug mode failed")
	}
	// enable full rt features
	ok = w.SetOption(sciter.SCITER_SET_SCRIPT_RUNTIME_FEATURES, sciter.ALLOW_FILE_IO|
		sciter.ALLOW_SOCKET_IO|
		sciter.ALLOW_EVAL|
		sciter.ALLOW_SYSINFO)
	if !ok {
		log.Println("enable full fetures failed")
	}
	// load file
	if len(flag.Args()) >= 1 {
		fp, err := filepath.Abs(flag.Arg(0))
		if err != nil {
			log.Fatal(err)
		}
		pageURI = fp
		if err = w.LoadFile(pageURI); err != nil {
			log.Fatal(err)
			return
		}
	} else {
		w.LoadHtml("Drag sciter htm file here, using F5 to refresh", "self")
	}
	// set handlers
	setupViewer(w)
	setCallbackHandlers(w)
	// show
	w.Show()
	w.Run()
}
