package sciter

/*
#include "sciter-x.h"

SCDOM_RESULT SCAPI RequestGetRequestType(HREQUEST rq, REQUEST_RQ_TYPE* pType);
*/
import "C"
import "unsafe"

// enum REQUEST_RESULT
const (
	REQUEST_PANIC = iota - 1 // e.g. not enough memory
	REQUEST_OK
	REQUEST_BAD_PARAM    // bad parameter
	REQUEST_FAILURE      // operation failed, e.g. index out of bounds
	REQUEST_NOTSUPPORTED // the platform does not support requested feature
)

// enum REQUEST_RQ_TYPE
const (
	RRT_GET = 1 + iota
	RRT_POST
	RRT_PUT
	RRT_DELETE

	RRT_FORCE_DWORD = 0xFFFFFFFF
)

// enum REQUEST_STATE
const (
	RS_PENDING = iota
	RS_SUCCESS // completed successfully
	RS_FAILURE // completed with failure

	RS_FORCE_DWORD = 0xFFFFFFFF
)

type Request struct {
	rq C.HREQUEST
}

func NewRequest(requestId C.HREQUEST) *Request {
	return &Request{requestId}
}

func (r *Request) GetRequestType() uint {
	var pType uint
	C.RequestGetRequestType(C.HREQUEST(r.rq), (*C.REQUEST_RQ_TYPE)(unsafe.Pointer(&pType)))
	return pType
}
