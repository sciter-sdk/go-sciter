package resx

import (
	"embed"
	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/resx"
	"github.com/sciter-sdk/go-sciter/window"
	"log"
)

//go:embed html/*
var f embed.FS

func main() {
	win, err := window.New(sciter.DefaultWindowCreateFlag, nil)
	if err != nil {
		log.Fatal(err)
	}
	sciter.SetOption(sciter.SCITER_SET_DEBUG_MODE, 1)
	//设置使用embed资源
	resx.HandleDataLoad(win.Sciter, f)

	win.LoadFile("resx://html/index.html")
	//设置标题
	win.SetTitle("内置资源")
	//显示窗口
	win.Show()
	//运行窗口，进入消息循环
	win.Run()
	log.Println(".....exit.....")
}
