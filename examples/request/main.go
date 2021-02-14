package main

import (
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/sciter-sdk/go-sciter"
	"github.com/sciter-sdk/go-sciter/window"
)

func main() {
	// NOTE: ALLOW_SOCKET_IO is necessary in order to make requests programmatically
	sciter.SetOption(sciter.SCITER_SET_SCRIPT_RUNTIME_FEATURES, sciter.ALLOW_SOCKET_IO)

	win, err := window.New(sciter.SW_MAIN|sciter.SW_TITLEBAR|sciter.SW_CONTROLS|sciter.SW_RESIZEABLE|sciter.SW_ENABLE_DEBUG, nil)
	if err != nil {
		log.Panic(err)
	}

	// setup asset routes
	win.SetCallback(&sciter.CallbackHandler{
		OnLoadData: func(params *sciter.ScnLoadData) int {
			return customOnLoadData(params)
		},
	})

	fullpath, err := filepath.Abs("index.html")
	if err != nil {
		log.Panic(err)
	}

	// load main page
	if err := win.LoadFile(fullpath); err != nil {
		log.Panic(err)
	}

	win.Show()
	win.Run()
}

func customOnLoadData(params *sciter.ScnLoadData) int {
	rq := sciter.WrapRequest(params.RequestId()) // get a new Request interface
	uri, _ := rq.Url()
	rqType, _ := rq.RequestType()
	rqDataType, _ := rq.RequestedDataType()
	if strings.HasPrefix(uri, "sync://") && rqType == sciter.RRT_GET && rqDataType == sciter.RT_DATA_IMAGE { // serve content synchronously
		path := uri[7:]
		f, err := os.Open(path)
		if err != nil {
			return sciter.LOAD_DISCARD
		}
		data, err := ioutil.ReadAll(f)
		if err != nil {
			return sciter.LOAD_DISCARD
		}
		rq.SetSucceeded(200, data)
	} else if strings.HasPrefix(uri, "async://") && rqType == sciter.RRT_GET && rqDataType == sciter.RT_DATA_IMAGE { // serve content asynchronously
		go func() { // load content from a goroutine
			path := uri[8:]
			f, err := os.Open(path)
			if err != nil {
				rq.SetFailed(404, nil)
				return
			}
			data, err := ioutil.ReadAll(f)
			if err != nil {
				rq.SetFailed(500, nil)
				return
			}
			//time.Sleep(time.Second * 3) // simulated network lag
			rq.SetSucceeded(200, data)
		}()

		// let sciter know we intend to load the data later
		return sciter.LOAD_DELAYED
	} else if strings.HasPrefix(uri, "chunked://") && rqType == sciter.RRT_GET && rqDataType == sciter.RT_DATA_IMAGE { // serve content asynchronously in chunks
		go func() { // load content from a goroutine
			path := uri[10:]
			f, err := os.Open(path)
			if err != nil {
				rq.SetFailed(404, nil)
				return
			}

			// load the data in chunks
			buf := make([]byte, 1<<16) // 65KB
			for {
				//time.Sleep(time.Millisecond * 100) // simulated network lag
				n, err := f.Read(buf)
				rq.AppendDataChunk(buf[:n])
				if err == io.EOF {
					rq.SetSucceeded(200, nil)
					return
				}
			}
		}()

		// let sciter know we intend to load the data later
		return sciter.LOAD_DELAYED
	}

	return sciter.LOAD_OK
}
