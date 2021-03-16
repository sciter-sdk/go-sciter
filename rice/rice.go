package rice

import (
	"log"
	"strings"

	rice "github.com/GeertJohan/go.rice"
	"github.com/sciter-sdk/go-sciter"
)

var (
	conf = rice.Config{
		LocateOrder: []rice.LocateMethod{
			rice.LocateWorkingDirectory,
			rice.LocateFS,
			rice.LocateAppended,
			rice.LocateEmbedded,
		},
	}
	boxmap = make(map[string]*rice.Box)
)

func OnLoadData(ld *sciter.ScnLoadData) int {
	req := sciter.WrapRequest(ld.RequestId())
	uri := ld.Uri()
	path := ""
	boxname := "."
	// log.Println("loading:", uri)
	// file:// or rice://
	if strings.HasPrefix(uri, "file://") || strings.HasPrefix(uri, "rice://") {
		path = uri[7:]
		ps := strings.Split(path, "/")
		if len(ps) >= 2 {
			boxname = ps[0]
			path = strings.Join(ps[1:], "/")
		}
	} else {
		// // do not handle schemes other than file:// or rice://
		return sciter.LOAD_OK
	}
	// log.Println("rice loading:", path, "in box:", boxname)
	// do box loading
	box, ok := boxmap[boxname]
	if !ok {
		var err error
		box, err = conf.FindBox(boxname)
		if err != nil {
			log.Println(err)
			// box locating failed, return to Sciter loading
			return sciter.LOAD_OK
		}
		boxmap[boxname] = box
	}
	// load resource from rice box
	dat, err := box.Bytes(path)
	if err != nil {
		// box locating failed, return to Sciter loading
		return sciter.LOAD_OK
	} else {
		// using rice found data
		req.SetSucceeded(200, dat)

		// log.Println("rice loaded:", path, "in box:", boxname)
	}
	return sciter.LOAD_OK
}

func HandleDataLoad(s *sciter.Sciter) {
	s.SetCallback(&sciter.CallbackHandler{
		OnLoadData: OnLoadData,
	})
}
