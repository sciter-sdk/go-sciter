#include "sciter-x.h"
#include "_cgo_export.h"

// typedef BOOL SC_CALLBACK SciterElementCallback(HELEMENT he, LPVOID param);

SBOOL SC_CALLBACK SciterElementCallback_cgo(HELEMENT he, LPVOID param)
{
    return goSciterElementCallback(he, param);
}

// typedef VOID SC_CALLBACK LPCBYTE_RECEIVER(LPCBYTE bytes, UINT num_bytes, LPVOID param);
VOID SC_CALLBACK LPCBYTE_RECEIVER_cgo(LPCBYTE bytes, UINT num_bytes, LPVOID param)
{
    goLPCBYTE_RECEIVER((BYTE*)bytes, num_bytes, param);
}

// typedef VOID SC_CALLBACK LPCWSTR_RECEIVER(LPCWSTR str, UINT str_length, LPVOID param);
VOID SC_CALLBACK LPCWSTR_RECEIVER_cgo(LPCWSTR str, UINT str_length, LPVOID param)
{
    goLPCWSTR_RECEIVER((WCHAR*)str, str_length, param);
}

// typedef VOID SC_CALLBACK LPCSTR_RECEIVER(LPCSTR str, UINT str_length, LPVOID param);
VOID SC_CALLBACK LPCSTR_RECEIVER_cgo(LPCSTR str, UINT str_length, LPVOID param)
{
    goLPCSTR_RECEIVER(str, str_length, param);
}

// typedef BOOL SC_CALLBACK ElementEventProc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms);
SBOOL SC_CALLBACK ElementEventProc_cgo(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms)
{
    return goElementEventProc(tag, he, evtg, prms);
}

// typedef UINT SC_CALLBACK SciterHostCallback(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam);
UINT SC_CALLBACK SciterHostCallback_cgo(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam)
{
    return goSciterHostCallback(pns, callbackParam);
}

// typedef VOID NATIVE_FUNCTOR_INVOKE(VOID* tag, UINT argc, const VALUE* argv, VALUE* retval); // retval may contain error definition
VOID NATIVE_FUNCTOR_INVOKE_cgo(VOID* tag, UINT argc, const VALUE* argv, VALUE* retval)
{
    goNATIVE_FUNCTOR_INVOKE(tag, argc, (VALUE*)argv, retval);
}

// typedef VOID NATIVE_FUNCTOR_RELEASE(VOID* tag);
VOID NATIVE_FUNCTOR_RELEASE_cgo(VOID* tag)
{
    goNATIVE_FUNCTOR_RELEASE(tag);
}

// typedef INT SC_CALLBACK ELEMENT_COMPARATOR(HELEMENT he1, HELEMENT he2, LPVOID param);

INT SC_CALLBACK ELEMENT_COMPARATOR_cgo(HELEMENT he1, HELEMENT he2, LPVOID param)
{
    return goELEMENT_COMPARATOR(he1, he2, param);
}

// typedef BOOL SC_CALLBACK KeyValueCallback(LPVOID param, const VALUE* pkey, const VALUE* pval);

SBOOL SC_CALLBACK KeyValueCallback_cgo(LPVOID param, const VALUE* pkey, const VALUE* pval)
{
    return goKeyValueCallback(param, (VALUE*)pkey, (VALUE*)pval);
}