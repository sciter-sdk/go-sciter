package sciter

import (
	"fmt"
	"runtime"
)

type NativeFunctor func(args ...*Value) *Value

// typedef VALUE SCITER_VALUE;
// type Scitervalue Value

type valueError struct {
	Result  VALUE_RESULT
	Message string
}

func (e *valueError) Error() string {
	return fmt.Sprintf("%s: %s", e.Result.String(), e.Message)
}

func newValueError(ret VALUE_RESULT, msg string) *valueError {
	return &valueError{
		Result:  ret,
		Message: msg,
	}
}

func wrapValueResult(r VALUE_RESULT, msg string) error {
	if r == VALUE_RESULT(HV_OK) {
		return nil
	}
	return newValueError(VALUE_RESULT(r), msg)
}

func valuePanic(result uint, message ...interface{}) {
	panic(&valueError{VALUE_RESULT(result), fmt.Sprint(message...)})
}

// Only supported basic types: int/bool/string/float/NativeFunctor/Value
//	 for creating array: a := NewValue(); a.SetIndex(0, 123)|a.Append(123)
//	 for creating map/object: obj := NewValue(); obj.Set("key", "value")
// The creating process would call ValueInit/VlaueClear automatically
func NewValue(val ...interface{}) *Value {
	v := new(Value)
	v.init()
	runtime.SetFinalizer(v, (*Value).finalize)
	if len(val) == 0 {
		return v
	}
	v.Assign(val[0])
	return v
}

func (v *Value) finalize() {
	// log.Println("finalizing value:", v)
	v.clear()
}

// release the object from sciter and go world
func (v *Value) Release() {
	runtime.SetFinalizer(v, nil)
	v.finalize()
}

// Assign go value to Sciter Value
// currently supported go types: bool integer float string and NativeFunctor
func (v *Value) Assign(val interface{}) {
	switch val.(type) {
	case string:
		s := val.(string)
		v.SetString(s)
	case int:
		i := val.(int)
		v.SetInt(i)
	case float64:
		f := val.(float64)
		// valueInit(this); valueFloatDataSet(this, v, T_FLOAT, 0)
		v.SetFloat(f)
	case bool:
		// value( bool v )           { valueInit(this); valueIntDataSet(this, v?1:0, T_BOOL, 0); }
		i := 0
		if val.(bool) {
			i = 1
		}
		v.SetInt(i)
	case float32:
		v.Assign(float64(val.(float32)))
	case uint:
		v.Assign(int(val.(uint)))
	case uint32:
		v.Assign(int(val.(uint32)))
	case uint64:
		v.Assign(int(val.(uint64)))
	case int32:
		v.Assign(int(val.(int32)))
	case int64:
		v.Assign(int(val.(int64)))
	case Value:
		vf := val.(Value)
		v.Copy(&vf)
	case *Value:
		v.Copy(val.(*Value))
	case NativeFunctor:
		v.SetNativeFunctor(val.(NativeFunctor))
	case func(args ...*Value) *Value:
		v.SetNativeFunctor((NativeFunctor)(val.(func(args ...*Value) *Value)))
	default:
		nf, ok := val.(NativeFunctor)
		if ok {
			v.SetNativeFunctor(nf)
		} else {
			panic("unsupported value type")
		}
	}
}

// bool is_undefined() const { return t == T_UNDEFINED; }
func (v *Value) IsUndefined() bool {
	return v.t == T_UNDEFINED
}

// bool is_bool() const { return t == T_BOOL; }
func (v *Value) IsBool() bool {
	return v.t == T_BOOL
}

// bool is_int() const { return t == T_INT; }
func (v *Value) IsInt() bool {
	return v.t == T_INT
}

// bool is_float() const { return t == T_FLOAT; }
func (v *Value) IsFloat() bool {
	return v.t == T_FLOAT
}

// bool is_string() const { return t == T_STRING; }
func (v *Value) IsString() bool {
	return v.t == T_STRING
}

// bool is_symbol() const { return t == T_STRING && u == UT_SYMBOL; }
func (v *Value) IsSymbol() bool {
	return v.t == T_STRING && v.t == UT_SYMBOL
}

// bool is_date() const { return t == T_DATE; }
func (v *Value) IsDate() bool {
	return v.t == T_DATE
}

// bool is_currency() const { return t == T_CURRENCY; }
func (v *Value) IsCurrency() bool {
	return v.t == T_CURRENCY
}

// bool is_map() const { return t == T_MAP; }
func (v *Value) IsMap() bool {
	return v.t == T_MAP
}

// bool is_array() const { return t == T_ARRAY; }
func (v *Value) IsArray() bool {
	return v.t == T_ARRAY
}

// bool is_function() const { return t == T_FUNCTION; }
func (v *Value) IsFunction() bool {
	return v.t == T_FUNCTION
}

// bool is_color() const { return t == T_COLOR; }
func (v *Value) IsColor() bool {
	return v.t == T_COLOR
}

// bool is_duration() const { return t == T_DURATION; }
func (v *Value) IsDuration() bool {
	return v.t == T_DURATION
}
// bool is_angle() const { return t == T_ANGLE; }
func (v *Value) IsAngle() bool {
	return v.t == T_ANGLE
}

// bool is_asset() const { return t == T_ASSET; }
func (v *Value) IsAsset() bool {
	return v.t == T_ASSET
}

// bool is_bytes() const { return t == T_BYTES; }
func (v *Value) IsByte() bool {
	return v.t == T_BYTES
}

// bool is_object() const { return t == T_OBJECT; }
func (v *Value) IsObject() bool {
	return v.t == T_OBJECT
}

// bool is_object_native() const   {  return t == T_OBJECT && u == UT_OBJECT_NATIVE; }
func (v *Value) IsObjectNative() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_NATIVE
}

// bool is_object_array() const    {  return t == T_OBJECT && u == UT_OBJECT_ARRAY; }
func (v *Value) IsObjectArray() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_ARRAY
}

// bool is_object_function() const {  return t == T_OBJECT && u == UT_OBJECT_FUNCTION; }
func (v *Value) IsObjectFunction() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_FUNCTION
}

// bool is_object_object() const   {  return t == T_OBJECT && u == UT_OBJECT_OBJECT; } // that is plain TS object
func (v *Value) IsObjectObject() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_OBJECT
}

// bool is_object_class() const    {  return t == T_OBJECT && u == UT_OBJECT_CLASS; }  // that is TS class
func (v *Value) IsObjectClass() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_CLASS
}

// bool is_object_error() const    {  return t == T_OBJECT && u == UT_OBJECT_ERROR; }  // that is TS error
func (v *Value) IsObjectE() bool {
	return v.t == T_OBJECT && v.u == UT_OBJECT_ERROR
}

// bool is_dom_element() const { return t == T_DOM_OBJECT; }
func (v *Value) IsDomElement() bool {
	return v.t == T_DOM_OBJECT
}

// bool is_null() const { return t == T_NULL; }
func (v *Value) IsNull() bool {
	return v.t == T_NULL
}

// // static value null() { value n; n.t = T_NULL; return n; }
func NullValue() *Value {
	v := new(Value)
	v.t = T_NULL
	v.init()
	return v
}

// bool is_nothing() const { return t == T_UNDEFINED && u == UT_NOTHING; }
func (v *Value) IsNothing() bool {
	return v.t == T_UNDEFINED && v.u == UT_NOTHING
}

// static value nothing() { value n; n.t = T_UNDEFINED; n.u = UT_NOTHING; return n; }
func NothingValue() *Value {
	v := new(Value)
	v.t = T_UNDEFINED
	v.u = UT_NOTHING
	v.init()
	return v
}
