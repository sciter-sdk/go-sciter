package main

import (
	"log"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	//创建新窗口
	//并设置窗口大小
	w, err := window.New(sciter.DefaultWindowCreateFlag, &sciter.Rect{200, 200, 500, 500})
	if err != nil {
		log.Fatal(err)
	}
	//加载文件
	w.LoadFile("demo5.html")
	//设置标题
	w.SetTitle("固定大小窗口")
	//显示窗口
	w.Show()
	//运行窗口，进入消息循环
	w.Run()
}
