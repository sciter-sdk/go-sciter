package main

import (
	"fmt"
	"log"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

//设置元素的处理程序
func setElementHandlers(root *sciter.Element) {
	btn1, _ := root.SelectById("btn1")
	//处理元素简单点击事件
	btn1.OnClick(func() {
		fmt.Println("btn1被点击了")
	})

	//这里给元素定义的方法是在tiscript中进行调用的
	//并且作为元素的属性进行访问
	btn1.DefineMethod("test", func(args ...*sciter.Value) *sciter.Value {
		//我们把从传入的参数打印出来
		for _, arg := range args {
			//String()把参数转换成字符串
			fmt.Print(arg.String() + " ")
		}

		//返回一个空值
		return sciter.NullValue()
	})
	btn2, _ := root.SelectById("btn2")
	//调用在tiscript中定义的方法
	data, _ := btn2.CallMethod("test2", sciter.NewValue("1"), sciter.NewValue("2"), sciter.NewValue("3"))
	//输出调用方法的返回值
	fmt.Println(data.String())
}

//设置回调
func setCallbackHandlers(w *window.Window) {
	//CallbackHandler是一个结构，里面定义了一些方法
	//你可以通过实现这些方法，自定义自已的回调
	cb := &sciter.CallbackHandler{
		//加载数据开始
		OnLoadData: func(p *sciter.ScnLoadData) int {
			//显示加载资源的uri
			fmt.Println("加载:", p.Uri())
			return sciter.LOAD_OK
		},
		//加载数据过程中
		OnDataLoaded: func(p *sciter.ScnDataLoaded) int {
			fmt.Println("加载中:", p.Uri())
			return sciter.LOAD_OK
		},
	}
	w.SetCallback(cb)
}

//定义函数
func setWinHandler(w *window.Window) {
	//定义函数，在tis脚本中需要通过view对象调用

	//定义inc函数，返回参数加1
	w.DefineFunction("inc", func(args ...*sciter.Value) *sciter.Value {
		return sciter.NewValue(args[0].Int() + 1)
	})
	//定义dec函数，返回参数减1
	w.DefineFunction("dec", func(args ...*sciter.Value) *sciter.Value {
		return sciter.NewValue(args[0].Int() - 1)
	})
}

//测试调用函数
func testCallFunc(w *window.Window) {
	//调用tis脚本中定义的函数
	data, _ := w.Call("sum", sciter.NewValue(10), sciter.NewValue(20))
	fmt.Println(data.String())
	root, _ := w.GetRootElement()
	//我们也可以指定元素调用函数
	data, _ = root.CallFunction("sum", sciter.NewValue(50), sciter.NewValue(100))
	fmt.Println(data.String())
}

func main() {
	//创建一个新窗口
	w, err := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	if err != nil {
		log.Fatal(err)
	}

	w.LoadFile("demo4.html")
	//设置标题
	w.SetTitle("事件处理")

	//设置回调处理程序
	setCallbackHandlers(w)
	//获取根元素
	root, _ := w.GetRootElement()
	//设置元素处理程序
	setElementHandlers(root)
	//设置窗口处理程序
	setWinHandler(w)
	//测试调用函数
	testCallFunc(w)

	//显示窗口
	w.Show()
	//运行窗口，进入消息循环
	w.Run()
}
