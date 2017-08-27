package main

import (
	"log"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	//create windows;创建window窗口
	//parm 1: style of window;参数一表示创建窗口的样式
	//SW_TITLEBAR :with titlebar;顶层窗口，有标题栏
	//SW_RESIZEABLE : resizeable; 可调整大小
	//SW_CONTROLS :has max, min button;有最小/最大按钮
	//SW_MAIN : main window, if it close, application will exit; 应用程序主窗口，关闭后其他所有窗口也会关闭
	//SW_ENABLE_DEBUG : debug;可以调试
	//parm 2: the rect of window;参数二表示创建窗口的矩形
	w, err := window.New(sciter.SW_TITLEBAR|
		sciter.SW_RESIZEABLE|
		sciter.SW_CONTROLS|
		sciter.SW_MAIN|
		sciter.SW_ENABLE_DEBUG,
		nil)
	if err != nil {
		log.Fatal(err)
	}
	//load file;加载文件
	w.LoadFile("demo1.html")
	//set title; 设置标题
	w.SetTitle("Hello, world")
	//show;显示窗口
	w.Show()
	//run, loop message;运行窗口，进入消息循环
	w.Run()
}
