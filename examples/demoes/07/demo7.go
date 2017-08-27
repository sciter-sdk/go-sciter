package main

import (
	"fmt"
	"log"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func defFunc(w *window.Window) {
	//注册dump函数方便在tis脚本中打印数据
	w.DefineFunction("dump", func(args ...*sciter.Value) *sciter.Value {
		for _, v := range args {
			fmt.Print(v.String() + " ")
		}
		fmt.Println()
		return sciter.NullValue()
	})
	//注册reg函数，用于处理注册逻辑，这里只是简单的把数据打印出来
	w.DefineFunction("reg", func(args ...*sciter.Value) *sciter.Value {
		for _, v := range args {
			fmt.Print(v.String() + " ")
		}
		fmt.Println()
		return sciter.NullValue()
	})
}

func main() {
	w, err := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	if err != nil {
		log.Fatal(err)
	}
	w.LoadFile("demo7.html")
	w.SetTitle("tiscript脚本学习")
	defFunc(w)
	w.Show()
	w.Run()
}
