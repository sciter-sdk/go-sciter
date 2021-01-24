package sciter

/*
#cgo CFLAGS: -Iinclude
#include "sciter-x.h"
*/
import "C"
import (
	"syscall"
	"unicode/utf16"
	"unsafe"
)

// Returns the utf-8 encoding of the utf-16 sequence s,
// with a terminating NUL removed.
func Utf16ToString(s *uint16) string {
	if s == nil {
		panic("null cstring")
	}
	us := make([]uint16, 0, 256)
	for p := uintptr(unsafe.Pointer(s)); ; p += 2 {
		u := *(*uint16)(unsafe.Pointer(p))
		if u == 0 {
			return string(utf16.Decode(us))
		}
		us = append(us, u)
	}
	return ""
}

func Utf16ToStringLength(s *uint16, length int) string {
	if s == nil {
		panic("null cstring")
	}
	us := make([]uint16, 0, 256)
	for p, i := uintptr(unsafe.Pointer(s)), 0; i < length; p, i = p+2, i+1 {
		u := *(*uint16)(unsafe.Pointer(p))
		us = append(us, u)
	}
	return string(utf16.Decode(us))
}

func StringToBytePtr(s string) *byte {
	bs := ([]byte)(s)
	return &bs[0]
}

// returns a UTF-16 string, including the trailing zero
func Utf16FromString(s string) ([]uint16, error) {
	for i := 0; i < len(s); i++ {
		if s[i] == 0 {
			return nil, syscall.EINVAL
		}
	}
	return utf16.Encode([]rune(s + "\x00")), nil
}

func StringToWcharPtr(s string) *C.WCHAR {
	return (*C.WCHAR)(unsafe.Pointer(StringToUTF16Ptr(s)))
}

func StringToUTF16Ptr(s string) *uint16 {
	us, _ := Utf16FromString(s)
	return &us[0]
}

func StringToUTF16PtrWithLen(s string) (*uint16, int) {
	us, _ := Utf16FromString(s)
	length := len(us) - 1
	return &us[0], length
}

func ByteCPtrToBytes(bp C.LPCBYTE, size C.UINT) []byte {
	bs := C.GoBytes(unsafe.Pointer(bp), C.INT(size))
	return bs
}

func BytePtrToBytes(bp *byte, size uint) []byte {
	bs := C.GoBytes(unsafe.Pointer(bp), C.INT(size))
	return bs
}
