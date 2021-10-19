 #include <sciter-x.h>

extern LPSciterRequestAPI rapi();


REQUEST_RESULT SCAPI RequestUse( HREQUEST rq ) { return rapi()->RequestUse(rq); }
REQUEST_RESULT SCAPI RequestUnUse( HREQUEST rq ) { return rapi()->RequestUnUse(rq); }
REQUEST_RESULT SCAPI RequestUrl( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestUrl(rq,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestContentUrl( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestContentUrl(rq,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetRequestType( HREQUEST rq, REQUEST_RQ_TYPE* pType ) { return rapi()->RequestGetRequestType(rq,pType); }
REQUEST_RESULT SCAPI RequestGetRequestedDataType( HREQUEST rq, SciterResourceType* pData ) { return rapi()->RequestGetRequestedDataType(rq,pData); }
REQUEST_RESULT SCAPI RequestGetReceivedDataType( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetReceivedDataType(rq,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetNumberOfParameters( HREQUEST rq, UINT* pNumber ) { return rapi()->RequestGetNumberOfParameters(rq,pNumber); }
REQUEST_RESULT SCAPI RequestGetNthParameterName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  ) { return rapi()->RequestGetNthParameterName(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetNthParameterValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param  ) { return rapi()->RequestGetNthParameterValue(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetTimes( HREQUEST rq, UINT* pStarted, UINT* pEnded ) { return rapi()->RequestGetTimes(rq,pStarted,pEnded); }
REQUEST_RESULT SCAPI RequestGetNumberOfRqHeaders( HREQUEST rq, UINT* pNumber ) { return rapi()->RequestGetNumberOfRqHeaders(rq,pNumber); }
REQUEST_RESULT SCAPI RequestGetNthRqHeaderName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetNthRqHeaderName(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetNthRqHeaderValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetNthRqHeaderValue(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetNumberOfRspHeaders( HREQUEST rq, UINT* pNumber ) { return rapi()->RequestGetNumberOfRspHeaders(rq,pNumber); }
REQUEST_RESULT SCAPI RequestGetNthRspHeaderName( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetNthRspHeaderName(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetNthRspHeaderValue( HREQUEST rq, UINT n, LPCWSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetNthRspHeaderValue(rq,n,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetCompletionStatus( HREQUEST rq, REQUEST_STATE* pState, UINT* pCompletionStatus ) { return rapi()->RequestGetCompletionStatus(rq,pState,pCompletionStatus); }
REQUEST_RESULT SCAPI RequestGetProxyHost( HREQUEST rq, LPCSTR_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetProxyHost(rq,rcv,rcv_param); }
REQUEST_RESULT SCAPI RequestGetProxyPort( HREQUEST rq, UINT* pPort ) { return rapi()->RequestGetProxyPort(rq,pPort); }
REQUEST_RESULT SCAPI RequestSetSucceeded( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength) { return rapi()->RequestSetSucceeded(rq,status,dataOrNull,dataLength); }
REQUEST_RESULT SCAPI RequestSetFailed( HREQUEST rq, UINT status, LPCBYTE dataOrNull, UINT dataLength ) { return rapi()->RequestSetFailed(rq,status,dataOrNull,dataLength); }
REQUEST_RESULT SCAPI RequestAppendDataChunk( HREQUEST rq, LPCBYTE data, UINT dataLength ) { return rapi()->RequestAppendDataChunk(rq,data,dataLength); }
REQUEST_RESULT SCAPI RequestSetRqHeader( HREQUEST rq, LPCWSTR name, LPCWSTR value ) { return rapi()->RequestSetRqHeader(rq,name,value); }
REQUEST_RESULT SCAPI RequestSetRspHeader( HREQUEST rq, LPCWSTR name, LPCWSTR value ) { return rapi()->RequestSetRspHeader(rq,name,value); }
REQUEST_RESULT SCAPI RequestSetReceivedDataType( HREQUEST rq, LPCSTR type ) { return rapi()->RequestSetReceivedDataType(rq,type); }
REQUEST_RESULT SCAPI RequestSetReceivedDataEncoding( HREQUEST rq, LPCSTR encoding ) { return rapi()->RequestSetReceivedDataEncoding(rq,encoding); }
REQUEST_RESULT SCAPI RequestGetData( HREQUEST rq, LPCBYTE_RECEIVER* rcv, LPVOID rcv_param ) { return rapi()->RequestGetData(rq,rcv,rcv_param); }
