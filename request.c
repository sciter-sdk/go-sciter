#include "sciter-x.h"

extern LPSciterRequestAPI rapi();

SCDOM_RESULT SCAPI RequestGetRequestType(HREQUEST rq, REQUEST_RQ_TYPE* pType) { return rapi()->RequestGetRequestType(rq,pType); }