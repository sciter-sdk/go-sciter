/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) Terra Informatica Software, Inc.
 */

 /*
 * Sciter's get resource request object - represents requests made by Element/View.request() functions.
 * 
 */


#ifndef __sciter_request_h__
#define __sciter_request_h__

#include "sciter-x-types.h"

#if defined(__cplusplus) 
  namespace html
  {
    struct request;
  }
  typedef html::request* HREQUEST;
#else
  typedef void*  HREQUEST;
#endif

typedef enum REQUEST_RESULT
{
  REQUEST_PANIC = -1, // e.g. not enough memory
  REQUEST_OK = 0,
  REQUEST_BAD_PARAM = 1,  // bad parameter
  REQUEST_FAILURE = 2,    // operation failed, e.g. index out of bounds
  REQUEST_NOTSUPPORTED = 3 // the platform does not support requested feature
} REQUEST_RESULT;

typedef enum REQUEST_RQ_TYPE
  {
    RRT_GET = 1,
    RRT_POST = 2,
    RRT_PUT = 3,
    RRT_DELETE = 4,

    RRT_FORCE_DWORD = 0xffffffff
  } REQUEST_RQ_TYPE;

typedef enum SciterResourceType
{
  RT_DATA_HTML = 0,
  RT_DATA_IMAGE = 1,
  RT_DATA_STYLE = 2,
  RT_DATA_CURSOR = 3,
  RT_DATA_SCRIPT = 4,
  RT_DATA_RAW = 5,
  RT_DATA_FONT,
  RT_DATA_SOUND,    // wav bytes
  RT_DATA_FORCE_DWORD = 0xffffffff
} SciterResourceType;

typedef enum REQUEST_STATE
  {
    RS_PENDING = 0,
    RS_SUCCESS = 1, // completed successfully
    RS_FAILURE = 2, // completed with failure

    RS_FORCE_DWORD = 0xffffffff
  } REQUEST_STATE;

struct SciterRequestAPI
{
  // a.k.a AddRef()
  REQUEST_RESULT
        SCFN(RequestUse)( HREQUEST rq );

  // a.k.a Release()
  REQUEST_RESULT
        SCFN(RequestUnUse)( HREQUEST rq );

  // get requested URL
  REQUEST_RESULT
        SCFN(RequestUrl)( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get real, content URL (after possible redirection)
  REQUEST_RESULT
        SCFN(RequestContentUrl)( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get requested data type
  REQUEST_RESULT
        SCFN(RequestGetRequestType)( HREQUEST rq, REQUEST_RQ_TYPE* pType );

  // get requested data type
  REQUEST_RESULT
        SCFN(RequestGetRequestedDataType)( HREQUEST rq, SciterResourceType* pData );

  // get received data type, string, mime type
  REQUEST_RESULT
        SCFN(RequestGetReceivedDataType)( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );


  // get number of request parameters passed
  REQUEST_RESULT
        SCFN(RequestGetNumberOfParameters)( HREQUEST rq, UINT* pNumber );

  // get nth request parameter name
  REQUEST_RESULT
        SCFN(RequestGetNthParameterName)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  );

  // get nth request parameter value
  REQUEST_RESULT
        SCFN(RequestGetNthParameterValue)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  );

  // get request times , ended - started = milliseconds to get the requst
  REQUEST_RESULT
        SCFN(RequestGetTimes)( HREQUEST rq, UINT* pStarted, UINT* pEnded );

  // get number of request headers
  REQUEST_RESULT
        SCFN(RequestGetNumberOfRqHeaders)( HREQUEST rq, UINT* pNumber );

  // get nth request header name 
  REQUEST_RESULT
        SCFN(RequestGetNthRqHeaderName)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get nth request header value 
  REQUEST_RESULT
        SCFN(RequestGetNthRqHeaderValue)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get number of response headers
  REQUEST_RESULT
        SCFN(RequestGetNumberOfRspHeaders)( HREQUEST rq, UINT* pNumber );

  // get nth response header name 
  REQUEST_RESULT
        SCFN(RequestGetNthRspHeaderName)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get nth response header value 
  REQUEST_RESULT
        SCFN(RequestGetNthRspHeaderValue)( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get completion status (CompletionStatus - http response code : 200, 404, etc.)
  REQUEST_RESULT
        SCFN(RequestGetCompletionStatus)( HREQUEST rq, REQUEST_STATE* pState, UINT* pCompletionStatus );

  // get proxy host
  REQUEST_RESULT
        SCFN(RequestGetProxyHost)( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param );

  // get proxy port
  REQUEST_RESULT
        SCFN(RequestGetProxyPort)( HREQUEST rq, UINT* pPort );

  // mark reequest as complete with status and data 
  REQUEST_RESULT
        SCFN(RequestSetSucceeded)( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength);
  
  // mark reequest as complete with failure and optional data 
  REQUEST_RESULT
        SCFN(RequestSetFailed)( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength );

  // append received data chunk 
  REQUEST_RESULT
        SCFN(RequestAppendDataChunk)( HREQUEST rq, LPCBYTE data, UINT dataLength );

  // set request header (single item)
  REQUEST_RESULT
        SCFN(RequestSetRqHeader)( HREQUEST rq, LPCWSTR name, LPCWSTR value );

  // set respone header (single item)
  REQUEST_RESULT
        SCFN(RequestSetRspHeader)( HREQUEST rq, LPCWSTR name, LPCWSTR value );

  // set received data type, string, mime type
  REQUEST_RESULT
        SCFN(RequestSetReceivedDataType)( HREQUEST rq, LPCSTR type );

  // set received data encoding, string
  REQUEST_RESULT
        SCFN(RequestSetReceivedDataEncoding)( HREQUEST rq, LPCSTR encoding );

  // get received (so far) data
  REQUEST_RESULT
        SCFN(RequestGetData)( HREQUEST rq, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param );

};

typedef struct SciterRequestAPI* LPSciterRequestAPI;

#endif
