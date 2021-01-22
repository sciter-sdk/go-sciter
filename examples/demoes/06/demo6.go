package main

import (
	"fmt"
	"log"
	"path/filepath"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func load(root *sciter.Element) {
	frame, _ := root.SelectById("frame")
	//load()类似jquery.load()，用于给元素加载指定内容
	//加载html内容
	frame.Load("http://www.qq.com", sciter.RT_DATA_HTML)

	//	txt, _ := root.SelectById("txt")
	//	//附加元素事件处理
	//	txt.AttachEventHandler(&sciter.EventHandler{
	//		//OnDataArrived 当资源被加载但未使用时调用
	//		//返回true，取消资源使用
	//		//返回false，遵循正常过程
	//		OnDataArrived: func(he *sciter.Element, params *sciter.DataArrivedParams) bool {
	//			//设置元素html
	//			he.SetHtml(string(params.Data()), sciter.SIH_REPLACE_CONTENT)
	//			return false
	//		},
	//	})
	//	//加载本地原始数据
	//	txt.Load("file:///D:/gopath/src/gui/1.txt", sciter.RT_DATA_RAW)

	img, _ := root.SelectById("img")
	img.AttachEventHandler(&sciter.EventHandler{
		//OnDataArrived 当资源被加载但未使用时调用
		OnDataArrived: func(he *sciter.Element, params *sciter.DataArrivedParams) bool {
			//设置属性，给img标签设置src
			he.SetAttr("src", params.Uri())
			return false
		},
	})
	img.Load("http://mat1.gtimg.com/www/images/qq2012/qqLogoFilter.png", sciter.RT_DATA_IMAGE)

	script, _ := root.SelectById("script")
	script.AttachEventHandler(&sciter.EventHandler{
		//OnDataArrived 当资源被加载但未使用时调用
		OnDataArrived: func(he *sciter.Element, params *sciter.DataArrivedParams) bool {
			fmt.Println(string(params.Data()))
			return false
		},
	})
	//加载脚本资源
	script.Load("http://apps.bdimg.com/libs/jquery/1.8.3/jquery.min.js", sciter.RT_DATA_SCRIPT)
}

func main() {
	w, err := window.New(sciter.DefaultWindowCreateFlag, sciter.DefaultRect)
	if err != nil {
		log.Fatal(err)
	}
	//加载文件
	fullpath, err := filepath.Abs("demo6.html")
	if err != nil {
		log.Fatal(err)
	}

	w.LoadFile(fullpath")
	//设置标题
	w.SetTitle("元素加载内容")
	//获取根元素
	root, _ := w.GetRootElement()
	//元素加载资源
	load(root)
	w.Show()
	w.Run()
}
