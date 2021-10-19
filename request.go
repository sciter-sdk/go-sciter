package sciter

/*
#include <sciter-x.h>
extern REQUEST_RESULT SCAPI RequestUse( HREQUEST rq );
extern REQUEST_RESULT SCAPI RequestUnUse( HREQUEST rq );
extern REQUEST_RESULT SCAPI RequestUrl( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestContentUrl( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetRequestType( HREQUEST rq, REQUEST_RQ_TYPE* pType );
extern REQUEST_RESULT SCAPI RequestGetRequestedDataType( HREQUEST rq, SciterResourceType* pData );
extern REQUEST_RESULT SCAPI RequestGetReceivedDataType( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetNumberOfParameters( HREQUEST rq, UINT* pNumber );
extern REQUEST_RESULT SCAPI RequestGetNthParameterName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  );
extern REQUEST_RESULT SCAPI RequestGetNthParameterValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  );
extern REQUEST_RESULT SCAPI RequestGetTimes( HREQUEST rq, UINT* pStarted, UINT* pEnded );
extern REQUEST_RESULT SCAPI RequestGetNumberOfRqHeaders( HREQUEST rq, UINT* pNumber );
extern REQUEST_RESULT SCAPI RequestGetNthRqHeaderName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetNthRqHeaderValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetNumberOfRspHeaders( HREQUEST rq, UINT* pNumber );
extern REQUEST_RESULT SCAPI RequestGetNthRspHeaderName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetNthRspHeaderValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetCompletionStatus( HREQUEST rq, REQUEST_STATE* pState, UINT* pCompletionStatus );
extern REQUEST_RESULT SCAPI RequestGetProxyHost( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );
extern REQUEST_RESULT SCAPI RequestGetProxyPort( HREQUEST rq, UINT* pPort );
extern REQUEST_RESULT SCAPI RequestSetSucceeded( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength);
extern REQUEST_RESULT SCAPI RequestSetFailed( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength );
extern REQUEST_RESULT SCAPI RequestAppendDataChunk( HREQUEST rq, LPCBYTE data, UINT dataLength );
extern REQUEST_RESULT SCAPI RequestSetRqHeader( HREQUEST rq, LPCWSTR name, LPCWSTR value );
extern REQUEST_RESULT SCAPI RequestSetRspHeader( HREQUEST rq, LPCWSTR name, LPCWSTR value );
extern REQUEST_RESULT SCAPI RequestSetReceivedDataType( HREQUEST rq, LPCSTR type );
extern REQUEST_RESULT SCAPI RequestSetReceivedDataEncoding( HREQUEST rq, LPCSTR encoding );
extern REQUEST_RESULT SCAPI RequestGetData( HREQUEST rq, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param );
*/
import "C"
import (
	"fmt"
	"runtime"
	"time"
	"unsafe"
)

/** Resource data type.
 *  Used by SciterDataReadyAsync() function.
 **/
type SciterResourceType uint32

// typedef enum SciterResourceType
const (
	RT_DATA_HTML SciterResourceType = iota
	RT_DATA_IMAGE
	RT_DATA_STYLE
	RT_DATA_CURSOR
	RT_DATA_SCRIPT
	RT_DATA_RAW
	RT_DATA_FONT
	RT_DATA_SOUND // wav bytes

	RT_DATA_FORCE_DWORD = 0xffffffff
)

type REQUEST_RESULT int32

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

type requestError struct {
	Result  REQUEST_RESULT
	Message string
}

func (e *requestError) Error() string {
	return fmt.Sprintf("%s: %s", e.Result.String(), e.Message)
}

func newRequestError(ret REQUEST_RESULT, msg string) *requestError {
	return &requestError{
		Result:  ret,
		Message: msg,
	}
}

func wrapRequestResult(r C.REQUEST_RESULT, msg string) error {
	if r == C.REQUEST_RESULT(REQUEST_OK) {
		return nil
	}
	return newRequestError(REQUEST_RESULT(r), msg)
}

var (
	BAD_HREQUEST = C.HREQUEST(unsafe.Pointer(uintptr(0)))
)

// Request represents a request issued by sciter
// e.g. el.request(...) or view.request(...)
type Request struct {
	handle C.HREQUEST
}

// WrapRequest wraps C.HREQUEST to a go side *Request, doing RequestUse/RequestUnUse automatically
func WrapRequest(requestId C.HREQUEST) *Request {
	r := &Request{
		handle: requestId,
	}
	r.use()
	runtime.SetFinalizer(r, (*Request).finalize)
	return r
}

func (r *Request) use() error {
	ret := C.RequestUse(r.handle)
	return wrapRequestResult(ret, "RequestUse")
}

func (r *Request) unUse() error {
	ret := C.RequestUnUse(r.handle)
	return wrapRequestResult(ret, "RequestUnUse")
}

func (r *Request) finalize() {
	r.unUse()
	r.handle = BAD_HREQUEST
}

func (r *Request) Url() (string, error) {
	var url string
	// args
	curl := C.LPVOID(unsafe.Pointer(&url))
	// cgo call
	ret := C.RequestUrl(r.handle, lpcstr_receiver, curl)
	return url, wrapRequestResult(ret, "RequestUrl")
}

func (r *Request) ContentUrl() (string, error) {
	var url string
	// args
	curl := C.LPVOID(unsafe.Pointer(&url))
	// cgo call
	ret := C.RequestContentUrl(r.handle, lpcstr_receiver, curl)
	return url, wrapRequestResult(ret, "RequestContentUrl")
}

func (r *Request) RequestType() (uint, error) {
	var requestType uint
	// args
	crequestType := (*C.REQUEST_RQ_TYPE)(unsafe.Pointer(&requestType))
	// cgo call
	ret := C.RequestGetRequestType(r.handle, crequestType)
	return requestType, wrapRequestResult(ret, "RequestGetRequestType")
}

func (r *Request) RequestedDataType() (SciterResourceType, error) {
	var resourceType SciterResourceType
	// args
	cresourceType := (*C.SciterResourceType)(unsafe.Pointer(&resourceType))
	// cgo call
	ret := C.RequestGetRequestedDataType(r.handle, cresourceType)
	return resourceType, wrapRequestResult(ret, "RequestGetRequestedDataType")
}

func (r *Request) ReceivedDataType() (string, error) {
	var dataType string
	// args
	cdataType := C.LPVOID(unsafe.Pointer(&dataType))
	// cgo call
	ret := C.RequestGetReceivedDataType(r.handle, lpcstr_receiver, cdataType)
	return dataType, wrapRequestResult(ret, "RequestGetReceivedDataType")
}

func (r *Request) NumberOfParameters() (uint, error) {
	var numParams uint
	// args
	cnumParams := (*C.UINT)(unsafe.Pointer(&numParams))
	// cgo call
	ret := C.RequestGetNumberOfParameters(r.handle, cnumParams)
	return numParams, wrapRequestResult(ret, "RequestGetNumberOfParameters")
}

func (r *Request) NthParameterName(idx uint) (string, error) {
	var name string
	// args
	cname := C.LPVOID(unsafe.Pointer(&name))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthParameterName(r.handle, cidx, lpcwstr_receiver, cname)
	return name, wrapRequestResult(ret, "RequestGetNthParameterName")
}

func (r *Request) NthParameterValue(idx uint) (string, error) {
	var value string
	// args
	cvalue := C.LPVOID(unsafe.Pointer(&value))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthParameterValue(r.handle, cidx, lpcwstr_receiver, cvalue)
	return value, wrapRequestResult(ret, "RequestGetNthParameterValue")
}

func (r *Request) Times() (time.Time, time.Time, error) {
	var started, ended uint
	var tStarted, tEnded time.Time
	// args
	cstarted := (*C.UINT)(unsafe.Pointer(&started))
	cended := (*C.UINT)(unsafe.Pointer(&ended))
	// cgo call
	ret := C.RequestGetTimes(r.handle, cstarted, cended)
	tStarted = time.Unix(int64(started), 0)
	tEnded = time.Unix(int64(ended), 0)
	return tStarted, tEnded, wrapRequestResult(ret, "RequestGetTimes")
}

func (r *Request) NumberOfRqHeaders() (uint, error) {
	var num uint
	// args
	cnum := (*C.UINT)(unsafe.Pointer(&num))
	// cgo call
	ret := C.RequestGetNumberOfRqHeaders(r.handle, cnum)
	return num, wrapRequestResult(ret, "RequestGetNumberOfRqHeaders")
}

func (r *Request) NthRqHeaderName(idx uint) (string, error) {
	var name string
	// args
	cname := C.LPVOID(unsafe.Pointer(&name))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthRqHeaderName(r.handle, cidx, lpcwstr_receiver, cname)
	return name, wrapRequestResult(ret, "RequestGetNthRqHeaderName")
}

func (r *Request) NthRqHeaderValue(idx uint) (string, error) {
	var val string
	// args
	cvalue := C.LPVOID(unsafe.Pointer(&val))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthRqHeaderValue(r.handle, cidx, lpcwstr_receiver, cvalue)
	return val, wrapRequestResult(ret, "RequestGetNthRqHeaderValue")
}

func (r *Request) NumberOfRspHeaders() (uint, error) {
	var num uint
	// args
	cnum := (*C.UINT)(unsafe.Pointer(&num))
	// cgo call
	ret := C.RequestGetNumberOfRspHeaders(r.handle, cnum)
	return num, wrapRequestResult(ret, "RequestGetNumberOfRspHeaders")
}

func (r *Request) NthRspHeaderName(idx uint) (string, error) {
	var name string
	// args
	cname := C.LPVOID(unsafe.Pointer(&name))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthRspHeaderName(r.handle, cidx, lpcwstr_receiver, cname)
	return name, wrapRequestResult(ret, "RequestGetNthRspHeaderName")
}

func (r *Request) NthRspHeaderValue(idx uint) (string, error) {
	var val string
	// args
	cvalue := C.LPVOID(unsafe.Pointer(&val))
	cidx := C.UINT(idx)
	// cgo call
	ret := C.RequestGetNthRspHeaderValue(r.handle, cidx, lpcwstr_receiver, cvalue)
	return val, wrapRequestResult(ret, "RequestGetNthRspHeaderValue")
}

func (r *Request) CompletionStatus() (uint, uint, error) {
	var state, status uint
	// args
	cstate := (*C.REQUEST_STATE)(unsafe.Pointer(&state))
	cstatus := (*C.UINT)(unsafe.Pointer(&status))
	// cgo call
	ret := C.RequestGetCompletionStatus(r.handle, cstate, cstatus)
	return state, status, wrapRequestResult(ret, "RequestGetCompletionStatus")
}

func (r *Request) ProxyHost() (string, error) {
	var host string
	// args
	chost := C.LPVOID(unsafe.Pointer(&host))
	// cgo call
	ret := C.RequestGetProxyHost(r.handle, lpcstr_receiver, chost)
	return host, wrapRequestResult(ret, "RequestGetProxyHost")
}

func (r *Request) ProxyPort() (uint, error) {
	var portno uint
	// args
	cportno := (*C.UINT)(unsafe.Pointer(&portno))
	// cgo call
	ret := C.RequestGetProxyPort(r.handle, cportno)
	return portno, wrapRequestResult(ret, "RequestGetProxyPort")
}

func (r *Request) SetSucceeded(status uint, data []byte) error {
	// args
	cstatus := C.UINT(status)
	var pData C.LPCBYTE
	if len(data) > 0 {
		pData = C.LPCBYTE(unsafe.Pointer(&data[0]))
	}
	cdataLength := C.UINT(len(data))

	//cgo call
	ret := C.RequestSetSucceeded(r.handle, cstatus, pData, cdataLength)
	return wrapRequestResult(ret, "RequestSetSucceeded")
}

func (r *Request) SetFailed(status uint, data []byte) error {
	// args
	cstatus := C.UINT(status)
	var pData C.LPCBYTE
	if len(data) > 0 {
		pData = C.LPCBYTE(unsafe.Pointer(&data[0]))
	}
	cdataLength := C.UINT(len(data))

	//cgo call
	ret := C.RequestSetFailed(r.handle, cstatus, pData, cdataLength)
	return wrapRequestResult(ret, "RequestSetFailed")
}

func (r *Request) AppendDataChunk(data []byte) error {
	// args
	var pData C.LPCBYTE
	if len(data) > 0 {
		pData = C.LPCBYTE(unsafe.Pointer(&data[0]))
	}
	cdataLength := C.UINT(len(data))

	//cgo call
	ret := C.RequestAppendDataChunk(r.handle, pData, cdataLength)
	return wrapRequestResult(ret, "RequestAppendDataChunk")
}

func (r *Request) SetRqHeader(name, value string) error {
	// args
	cname := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(name)))
	cvalue := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(value)))
	// cgo call
	ret := C.RequestSetRqHeader(r.handle, cname, cvalue)
	return wrapRequestResult(ret, "RequestSetRqHeader")
}

func (r *Request) SetRspHeader(name, value string) error {
	// args
	cname := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(name)))
	cvalue := C.LPCWSTR(unsafe.Pointer(StringToWcharPtr(value)))
	// cgo call
	ret := C.RequestSetRspHeader(r.handle, cname, cvalue)
	return wrapRequestResult(ret, "RequestSetRspHeader")
}

func (r *Request) SetReceivedDataType(dataType string) error {
	// args
	cdataType := C.LPCSTR(unsafe.Pointer(StringToBytePtr(dataType)))
	// cgo call
	ret := C.RequestSetReceivedDataType(r.handle, cdataType)
	return wrapRequestResult(ret, "RequestSetReceivedDataType")
}

func (r *Request) RequestSetReceivedDataEncoding(encoding string) error {
	// args
	cencoding := C.LPCSTR(unsafe.Pointer(StringToBytePtr(encoding)))
	// cgo call
	ret := C.RequestSetReceivedDataEncoding(r.handle, cencoding)
	return wrapRequestResult(ret, "RequestSetReceivedDataEncoding")
}

func (r *Request) Data() ([]byte, error) {
	var buf []byte
	// args
	cbuf := C.LPVOID(unsafe.Pointer(&buf))
	// cgo call
	ret := C.RequestGetData(r.handle, lpcbyte_receiver, cbuf)
	return buf, wrapRequestResult(ret, "RequestGetData")
}
