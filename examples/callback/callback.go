package main

import (
	"log"

	"github.com/oskca/sciter"
	"github.com/oskca/sciter/window"
)

func main() {
	w, err := window.New(sciter.SW_TITLEBAR|sciter.SW_RESIZEABLE|sciter.SW_CONTROLS|sciter.SW_MAIN|sciter.SW_ENABLE_DEBUG, nil)
	if err != nil {
		log.Fatal("Create Window Error: ", err)
	}
	w.LoadFile("index.html")
	setEventHandler(w)
	w.Show()
	w.Run()
}

func setEventHandler(w *window.Window) {
	w.DefineFunction("getNetInformation", func(args ...*sciter.Value) *sciter.Value {
		log.Println("Args Length:", len(args))
		log.Println("Arg 0:", args[0], args[0].IsInt())
		log.Println("Arg 1:", args[1], args[1].IsString())
		log.Println("Arg 2: IsFunction", args[2], args[2].IsFunction())
		log.Println("Arg 2: IsObjecFunction", args[2], args[2].IsObjecFunction())
		fn := args[2]
		fn.Invoke(sciter.NullValue(), "[Native Script]", sciter.NewValue("OK"))
		ret := sciter.NewValue()
		ret.Set("ip", sciter.NewValue("127.0.0.1"))
		return ret
	})
}
