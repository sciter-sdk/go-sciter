package main

import (
	"flag"
	"github.com/oskca/sciter"
	"github.com/oskca/sciter/window"
	"log"
)

func load(w *window.Window, url string) {
	root, err := w.GetRootElement()
	if err != nil {
		log.Panic(err)
	}
	els, err := root.Select("#url")
	if err != nil {
		log.Fatal("select:", err)
	}
	log.Println("els:", els)
	span, _ := root.SelectById("url")
	if span == nil {
		log.Panic("span is nil")
	}
	content, _ := root.SelectById("content")
	if content == nil {
		log.Panic("content is nil")
	}
	text, err := span.Text()
	if err != nil {
		log.Panic(err)
	}
	log.Println("span:", text)
	err = span.SetText(url)
	if err != nil {
		log.Panic(err)
	}

	content.AttachEventHandler(&sciter.EventHandler{
		OnDataArrived: func(he *sciter.Element, params *sciter.DataArrivedParams) bool {
			log.Println("uri:", params.Uri(), len(params.Data()))
			return false
		},
	})

	err = content.Load(url, sciter.RT_DATA_HTML)
	if err != nil {
		log.Panic(err)
	}
}

func testArray() {
	v := sciter.NewValue()
	// v.SetIndex(0, sciter.NewValue(100))
	log.Println("v.len", v.Length())
	v.Append(105)
	log.Println("v.len", v.Length(), v.IsArray())
	v.Append(205)
	log.Println("v.len", v.Length())
	v.Append(200)
	log.Println("v.len", v.Length())
	rv := v.Index(0)
	log.Println("test Value.Index:", rv)
}

func testMap() {
	m := sciter.NewValue()
	m.Set("key", "asdf")
	log.Println("m.len", m.Length(), m.IsMap())
	m.Set("int", 123)
	log.Println("m:", m.Get("key"), m.Get("int"))
}

func test() {
	testArray()
	testMap()
}

func main() {
	log.Println("Sciter Version:", sciter.VersionAsString())
	flag.Parse()
	if flag.NArg() == 0 {
		log.Fatalln("at least one Sciter compatible page url is needed")
	}
	log.Println(flag.Arg(0))
	w, err := window.New(sciter.SW_ENABLE_DEBUG|sciter.SW_GLASSY, sciter.DefaultRect)
	if err != nil {
		log.Fatal(err)
	}

	w.SetTitle("Download Element Content")
	w.LoadHtml("<html><body><span id='url'>Url To Load</span><frame id='content'></frame></body></html>", "/")
	test()
	load(w, flag.Arg(0))
	w.Show()
	w.Run()
}
