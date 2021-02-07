package sciter

/*
#include "sciter-x.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// enum SCITER_CREATE_WINDOW_FLAGS {
//    SW_CHILD      = (1 << 0), // child window only, if this flag is set all other flags ignored
//    SW_TITLEBAR   = (1 << 1), // toplevel window, has titlebar
//    SW_RESIZEABLE = (1 << 2), // has resizeable frame
//    SW_TOOL       = (1 << 3), // is tool window
//    SW_CONTROLS   = (1 << 4), // has minimize / maximize buttons
//    SW_GLASSY     = (1 << 5), // glassy window ( DwmExtendFrameIntoClientArea on windows )
//    SW_ALPHA      = (1 << 6), // transparent window ( e.g. WS_EX_LAYERED on Windows )
//    SW_MAIN       = (1 << 7), // main window of the app, will terminate the app on close
//    SW_POPUP      = (1 << 8), // the window is created as topmost window.
//    SW_ENABLE_DEBUG = (1 << 9), // make this window inspector ready
//    SW_OWNS_VM      = (1 << 10), // it has its own script VM
// };
type WindowCreationFlag uint32

const (
	SW_CHILD        WindowCreationFlag = (1 << iota) // child window only, if this flag is set all other flags ignored
	SW_TITLEBAR                                      // toplevel window, has titlebar
	SW_RESIZEABLE                                    // has resizeable frame
	SW_TOOL                                          // is tool window
	SW_CONTROLS                                      // has minimize / maximize buttons
	SW_GLASSY                                        // glassy window ( DwmExtendFrameIntoClientArea on windows )
	SW_ALPHA                                         // transparent window ( e.g. WS_EX_LAYERED on Windows )
	SW_MAIN                                          // main window of the app, will terminate the app on close
	SW_POPUP                                         // the window is created as topmost window.
	SW_ENABLE_DEBUG                                  // make this window inspector ready
	SW_OWNS_VM                                       // it has its own script VM
)

var (
	// create an resizable main window with minimize/maximize controls
	// linux must use this combination to create the main window correctly
	DefaultWindowCreateFlag = SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG
)

// enum OUTPUT_SUBSYTEMS
const (
	OT_DOM  = iota // html parser & runtime
	OT_CSSS        // csss! parser & runtime
	OT_CSS         // css parser
	OT_TIS         // TIS parser & runtime
)

// enum OUTPUT_SEVERITY
const (
	OS_INFO = iota
	OS_WARNING
	OS_ERROR
)

// enum EVENT_GROUPS
const (
	HANDLE_INITIALIZATION = 0x0000 /** attached/detached */
	HANDLE_MOUSE          = 0x0001 /** mouse events */
	HANDLE_KEY            = 0x0002 /** key events */
	HANDLE_FOCUS          = 0x0004 /** focus events if this flag is set it also means that element it attached to is focusable */
	HANDLE_SCROLL         = 0x0008 /** scroll events */
	HANDLE_TIMER          = 0x0010 /** timer event */
	HANDLE_SIZE           = 0x0020 /** size changed event */
	HANDLE_DRAW           = 0x0040 /** drawing request (event) */
	HANDLE_DATA_ARRIVED   = 0x080  /** requested data () has been delivered */
	HANDLE_BEHAVIOR_EVENT = 0x0100 /** logical synthetic events:
	BUTTON_CLICK HYPERLINK_CLICK etc.
	a.k.a. notifications from intrinsic behaviors */
	HANDLE_METHOD_CALL           = 0x0200 /** behavior specific methods */
	HANDLE_SCRIPTING_METHOD_CALL = 0x0400 /** behavior specific methods */
	HANDLE_TISCRIPT_METHOD_CALL  = 0x0800 /** behavior specific methods using direct tiscript::value's */

	HANDLE_EXCHANGE = 0x1000 /** system drag-n-drop */
	HANDLE_GESTURE  = 0x2000 /** touch input events */
	HANDLE_SOM      = 0x8000 /** som_asset_t request */

	HANDLE_ALL = 0xFFFF /* all of them */

	SUBSCRIPTIONS_REQUEST = 0xFFFFFFFF /** special value for getting subscription flags */
)

type PhaseMask uint32

// enum PHASE_MASK
const (
	BUBBLING        = 0       // bubbling (emersion) phase
	SINKING         = 0x8000  // capture (immersion) phase, this flag is or'ed with EVENTS codes below
	HANDLED         = 0x10000 // a bubbling event consumed by some element
	SINKING_HANDLED = 0x18000 // a sinking event consumed by some child element
	// see: http://www.w3.org/TR/xml-events/Overview.html#s_intro
)

type MouseButton uint32

// enum MOUSE_BUTTONS
const (
	MAIN_MOUSE_BUTTON   MouseButton = 1 //aka left button
	PROP_MOUSE_BUTTON   MouseButton = 2 //aka right button
	MIDDLE_MOUSE_BUTTON MouseButton = 4
)

type KeyboardState uint32

// enum KEYBOARD_STATES
const (
	CONTROL_KEY_PRESSED KeyboardState = 0x1
	SHIFT_KEY_PRESSED   KeyboardState = 0x2
	ALT_KEY_PRESSED     KeyboardState = 0x4
)

// parameters of evtg == HANDLE_INITIALIZATION
// enum INITIALIZATION_EVENTS
const (
	BEHAVIOR_DETACH = 0
	BEHAVIOR_ATTACH = 1
)

type DraggingType uint32

// enum DRAGGING_TYPE
const (
	NO_DRAGGING = iota
	DRAGGING_MOVE
	DRAGGING_COPY
)

type MouseEvent uint32

// parameters of evtg == HANDLE_MOUSE
// enum MOUSE_EVENTS
const (
	MOUSE_ENTER MouseEvent = iota
	MOUSE_LEAVE
	MOUSE_MOVE
	MOUSE_UP
	MOUSE_DOWN
	MOUSE_DCLICK
	MOUSE_WHEEL
	MOUSE_TICK // mouse pressed ticks
	MOUSE_IDLE // mouse stay idle for some time

	DROP         // item dropped target is that dropped item
	DRAG_ENTER   // drag arrived to the target element that is one of current drop targets.
	DRAG_LEAVE   // drag left one of current drop targets. target is the drop target element.
	DRAG_REQUEST // drag src notification before drag start. To cancel - return true from handler.

	MOUSE_CLICK MouseEvent = 0xFF // mouse click event

	DRAGGING MouseEvent = 0x100 // This flag is 'ORed' with MOUSE_ENTER..MOUSE_DOWN codes if dragging operation is in effect.
	// E.g. event DRAGGING | MOUSE_MOVE is sent to underlying DOM elements while dragging.
)

type CursorType uint32

// enum CURSOR_TYPE
const (
	CURSOR_ARROW       CursorType = iota //0
	CURSOR_IBEAM                         //1
	CURSOR_WAIT                          //2
	CURSOR_CROSS                         //3
	CURSOR_UPARROW                       //4
	CURSOR_SIZENWSE                      //5
	CURSOR_SIZENESW                      //6
	CURSOR_SIZEWE                        //7
	CURSOR_SIZENS                        //8
	CURSOR_SIZEALL                       //9
	CURSOR_NO                            //10
	CURSOR_APPSTARTING                   //11
	CURSOR_HELP                          //12
	CURSOR_HAND                          //13
	CURSOR_DRAG_MOVE                     //14
	CURSOR_DRAG_COPY                     //15
)

type KeyEvent uint32

// enum KEY_EVENTS
const (
	KEY_DOWN KeyEvent = iota
	KEY_UP
	KEY_CHAR
)

type FocusEvent uint32

// enum FOCUS_EVENTS
const (
	FOCUS_LOST FocusEvent = iota
	FOCUS_GOT
)

type ScrollEvent uint32

// enum SCROLL_EVENTS
const (
	SCROLL_HOME ScrollEvent = iota
	SCROLL_END
	SCROLL_STEP_PLUS
	SCROLL_STEP_MINUS
	SCROLL_PAGE_PLUS
	SCROLL_PAGE_MINUS
	SCROLL_POS
	SCROLL_SLIDER_RELEASED
	SCROLL_CORNER_PRESSED
	SCROLL_CORNER_RELEASED
)

type GestureCmd uint32

// enum GESTURE_CMD
const (
	GESTURE_REQUEST GestureCmd = iota // return true and fill flags if it will handle gestures.
	GESTURE_ZOOM                      // The zoom gesture.
	GESTURE_PAN                       // The pan gesture.
	GESTURE_ROTATE                    // The rotation gesture.
	GESTURE_TAP1                      // The tap gesture.
	GESTURE_TAP2                      // The two-finger tap gesture.
)

type GestureState uint32

// enum GESTURE_STATE
const (
	GESTURE_STATE_BEGIN   GestureState = 1 // starts
	GESTURE_STATE_INERTIA GestureState = 2 // events generated by inertia processor
	GESTURE_STATE_END     GestureState = 4 // end last event of the gesture sequence
)

type GestureTypeFlag uint32

// enum GESTURE_TYPE_FLAGS // requested
const (
	GESTURE_FLAG_ZOOM                          GestureTypeFlag = 0x0001
	GESTURE_FLAG_ROTATE                        GestureTypeFlag = 0x0002
	GESTURE_FLAG_PAN_VERTICAL                  GestureTypeFlag = 0x0004
	GESTURE_FLAG_PAN_HORIZONTALGestureTypeFlag                 = 0x0008
	GESTURE_FLAG_TAP1                          GestureTypeFlag = 0x0010 // press & tap
	GESTURE_FLAG_TAP2                          GestureTypeFlag = 0x0020 // two fingers tap

	GESTURE_FLAG_PAN_WITH_GUTTER  GestureTypeFlag = 0x4000 // PAN_VERTICAL and PAN_HORIZONTAL modifiers
	GESTURE_FLAG_PAN_WITH_INERTIA GestureTypeFlag = 0x8000 //
	GESTURE_FLAGS_ALL             GestureTypeFlag = 0xFFFF //
)

type DrawEvent uint32

// enum DRAW_EVENTS
const (
	DRAW_CONTENT    DrawEvent = 1
	DRAW_FOREGROUND DrawEvent = 2
)

// enum CONTENT_CHANGE_BITS {  // for CONTENT_CHANGED reason
const (
	CONTENT_ADDED   = 0x01
	CONTENT_REMOVED = 0x02
)

type BehaviorEvent uint32

// enum BEHAVIOR_EVENTS
const (
	BUTTON_CLICK             BehaviorEvent = 0 // click on button
	BUTTON_PRESS             BehaviorEvent = 1 // mouse down or key down in button
	BUTTON_STATE_CHANGED     BehaviorEvent = 2 // checkbox/radio/slider changed its state/value
	EDIT_VALUE_CHANGING      BehaviorEvent = 3 // before text change
	EDIT_VALUE_CHANGED       BehaviorEvent = 4 // after text change
	SELECT_SELECTION_CHANGED BehaviorEvent = 5 // selection in <select> changed
	SELECT_STATE_CHANGED     BehaviorEvent = 6 // node in select expanded/collapsed heTarget is the node

	POPUP_REQUEST BehaviorEvent = 7 // request to show popup just received
	//     here DOM of popup element can be modifed.
	POPUP_READY BehaviorEvent = 8 // popup element has been measured and ready to be shown on screen
	//     here you can use functions like ScrollToView.
	POPUP_DISMISSED BehaviorEvent = 9 // popup element is closed
	//     here DOM of popup element can be modifed again - e.g. some items can be removed
	//     to free memory.

	MENU_ITEM_ACTIVE BehaviorEvent = 0xA // menu item activated by mouse hover or by keyboard
	MENU_ITEM_CLICK  BehaviorEvent = 0xB // menu item click
	//   BEHAVIOR_EVENT_PARAMS structure layout
	//   BEHAVIOR_EVENT_PARAMS.cmd - MENU_ITEM_CLICK/MENU_ITEM_ACTIVE
	//   BEHAVIOR_EVENT_PARAMS.heTarget - owner(anchor) of the menu
	//   BEHAVIOR_EVENT_PARAMS.he - the menu item presumably <li> element
	//   BEHAVIOR_EVENT_PARAMS.reason - BY_MOUSE_CLICK | BY_KEY_CLICK

	CONTEXT_MENU_REQUEST BehaviorEvent = 0x10 // "right-click" BEHAVIOR_EVENT_PARAMS::he is current popup menu HELEMENT being processed or NULL.
	// application can provide its own HELEMENT here (if it is NULL) or modify current menu element.

	VISIUAL_STATUS_CHANGED  BehaviorEvent = 0x11 // broadcast notification sent to all elements of some container being shown or hidden
	DISABLED_STATUS_CHANGED BehaviorEvent = 0x12 // broadcast notification sent to all elements of some container that got new value of :disabled state

	POPUP_DISMISSING BehaviorEvent = 0x13 // popup is about to be closed

	CONTENT_CHANGED BehaviorEvent = 0x15 // content has been changed is posted to the element that gets content changed  reason is combination of CONTENT_CHANGE_BITS.
	// target == NULL means the window got new document and this event is dispatched only to the window.

	// "grey" event codes  - notfications from behaviors from this SDK
	HYPERLINK_CLICK BehaviorEvent = 0x80 // hyperlink click

	//TABLE_HEADER_CLICK            // click on some cell in table header
	//                               //     target = the cell
	//                               //     reason = index of the cell (column number 0..n)
	//TABLE_ROW_CLICK               // click on data row in the table target is the row
	//                               //     target = the row
	//                               //     reason = index of the row (fixed_rows..n)
	//TABLE_ROW_DBL_CLICK           // mouse dbl click on data row in the table target is the row
	//                               //     target = the row
	//                               //     reason = index of the row (fixed_rows..n)
)

const (
	ELEMENT_COLLAPSED BehaviorEvent = iota + 0x90 // element was collapsed so far only behavior:tabs is sending these two to the panels
	ELEMENT_EXPANDED                              // element was expanded

	ACTIVATE_CHILD // activate (select) child
	// used for example by accesskeys behaviors to send activation request e.g. tab on behavior:tabs.

	//DO_SWITCH_TAB = ACTIVATE_CHILD// command to switch tab programmatically handled by behavior:tabs
	//                               // use it as HTMLayoutPostEvent(tabsElementOrItsChild DO_SWITCH_TAB tabElementToShow 0);

	INIT_DATA_VIEW // request to virtual grid to initialize its view

	ROWS_DATA_REQUEST // request from virtual grid to data source behavior to fill data in the table
	// parameters passed throug DATA_ROWS_PARAMS structure.

	UI_STATE_CHANGED // ui state changed observers shall update their visual states.
	// is sent for example by behavior:richtext when caret position/selection has changed.

	FORM_SUBMIT // behavior:form detected submission event. BEHAVIOR_EVENT_PARAMS::data field contains data to be posted.
	// BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about
	// to be submitted. You can modify the data or discard submission by returning true from the handler.
	FORM_RESET // behavior:form detected reset event (from button type=reset). BEHAVIOR_EVENT_PARAMS::data field contains data to be reset.
	// BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about
	// to be rest. You can modify the data or discard reset by returning true from the handler.

	DOCUMENT_COMPLETE // document in behavior:frame or root document is complete.

	HISTORY_PUSH // requests to behavior:history (commands)
	HISTORY_DROP
	HISTORY_PRIOR
	HISTORY_NEXT
	HISTORY_STATE_CHANGED // behavior:history notification - history stack has changed

	CLOSE_POPUP     // close popup request
	REQUEST_TOOLTIP // request tooltip evt.source <- is the tooltip element.

	ANIMATION BehaviorEvent = 0xA0 // animation started (reason=1) or ended(reason=0) on the element.

	DOCUMENT_CREATED       BehaviorEvent = 0xC0 // document created script namespace initialized. target -> the document
	DOCUMENT_CLOSE_REQUEST BehaviorEvent = 0xC1 // document is about to be closed to cancel closing do: evt.data = sciter::value("cancel");
	DOCUMENT_CLOSE         BehaviorEvent = 0xC2 // last notification before document removal from the DOM
	DOCUMENT_READY         BehaviorEvent = 0xC3 // document has got DOM structure styles and behaviors of DOM elements. Script loading run is complete at this moment.

	VIDEO_INITIALIZED BehaviorEvent = 0xD1 // <video> "ready" notification
	VIDEO_STARTED     BehaviorEvent = 0xD2 // <video> playback started notification
	VIDEO_STOPPED     BehaviorEvent = 0xD3 // <video> playback stoped/paused notification
	VIDEO_BIND_RQ     BehaviorEvent = 0xD4 // <video> request for frame source binding
	//   If you want to provide your own video frames source for the given target <video> element do the following:
	//   1. Handle and consume this VIDEO_BIND_RQ request
	//   2. You will receive second VIDEO_BIND_RQ request/event for the same <video> element
	//      but this time with the 'reason' field set to an instance of sciter::video_destination interface.
	//   3. add_ref() it and store it for example in worker thread producing video frames.
	//   4. call sciter::video_destination::start_streaming(...) providing needed parameters
	//      call sciter::video_destination::render_frame(...) as soon as they are available
	//      call sciter::video_destination::stop_streaming() to stop the rendering (a.k.a. end of movie reached)

	FIRST_APPLICATION_EVENT_CODE = 0x100
	// all custom event codes shall be greater
	// than this number. All codes below this will be used
	// solely by application - HTMLayout will not intrepret it
	// and will do just dispatching.
	// To send event notifications with  these codes use
	// HTMLayoutSend/PostEvent API.
)

type EventReason uint

// enum EVENT_REASON
const (
	BY_MOUSE_CLICK EventReason = iota
	BY_KEY_CLICK
	SYNTHESIZED // synthesized programmatically generated.
	BY_MOUSE_ON_ICON
)

type EditChangedReason uint

// enum EDIT_CHANGED_REASON
const (
	BY_INS_CHAR  EditChangedReason = iota // single char insertion
	BY_INS_CHARS                          // character range insertion clipboard
	BY_DEL_CHAR                           // single char deletion
	BY_DEL_CHARS                          // character range deletion (selection)
	BY_UNDO_REDO                          // undo/redo
)

type BehaviorMethodIdentifier uint32

// enum BEHAVIOR_METHOD_IDENTIFIERS
const (
	DO_CLICK BehaviorMethodIdentifier = iota
	GET_TEXT_VALUE
	SET_TEXT_VALUE
	// p - TEXT_VALUE_PARAMS

	TEXT_EDIT_GET_SELECTION
	// p - TEXT_EDIT_SELECTION_PARAMS

	TEXT_EDIT_SET_SELECTION
	// p - TEXT_EDIT_SELECTION_PARAMS

	// Replace selection content or insert text at current caret position.
	// Replaced text will be selected.
	TEXT_EDIT_REPLACE_SELECTION
	// p - TEXT_EDIT_REPLACE_SELECTION_PARAMS

	// Set value of type="vscrollbar"/"hscrollbar"
	SCROLL_BAR_GET_VALUE
	SCROLL_BAR_SET_VALUE

	TEXT_EDIT_GET_CARET_POSITION
	TEXT_EDIT_GET_SELECTION_TEXT // p - TEXT_SELECTION_PARAMS
	TEXT_EDIT_GET_SELECTION_HTML // p - TEXT_SELECTION_PARAMS
	TEXT_EDIT_CHAR_POS_AT_XY     // p - TEXT_EDIT_CHAR_POS_AT_XY_PARAMS

	IS_EMPTY  BehaviorMethodIdentifier = 0xFC // p - IS_EMPTY_PARAMS // set VALUE_PARAMS::is_empty (false/true) reflects :empty state of the element.
	GET_VALUE BehaviorMethodIdentifier = 0xFD // p - VALUE_PARAMS
	SET_VALUE BehaviorMethodIdentifier = 0xFE // p - VALUE_PARAMS

	FIRST_APPLICATION_METHOD_ID BehaviorMethodIdentifier = 0x100
)

// enum VALUE_TYPE
const (
	T_UNDEFINED = iota
	T_NULL
	T_BOOL
	T_INT
	T_FLOAT
	T_STRING
	T_DATE     // INT64 - contains a 64-bit value representing the number of 100-nanosecond intervals since January 1 1601 (UTC) a.k.a. FILETIME on Windows
	T_CURRENCY // INT64 - 14.4 fixed number. E.g. dollars = int64 / 10000;
	T_LENGTH   // length units value is int or float units are VALUE_UNIT_TYPE
	T_ARRAY
	T_MAP
	T_FUNCTION
	T_BYTES      // sequence of bytes - e.g. image data
	T_OBJECT     // scripting object proxy (TISCRIPT/SCITER)
	T_DOM_OBJECT // DOM object (CSSS!) use get_object_data to get HELEMENT
	T_RESOURCE   // 15 - other thing derived from tool::resource
	T_RANGE      // 16 - N..M, integer range.
	T_DURATION   // double, seconds
	T_ANGLE      // double, radians
	T_COLOR      // [unsigned] INT, ABGR
	T_ENUM
	T_ASSET // sciter::om::iasset* add_ref'ed pointer
)

// enum VALUE_UNIT_TYPE
const (
	UT_EM     = 1 //height of the element's font.
	UT_EX     = 2 //height of letter 'x'
	UT_PR     = 3 //%
	UT_SP     = 4 //%% "springs" a.k.a. flex units
	reserved1 = 5
	reserved2 = 6
	UT_PX     = 7  //pixels
	UT_IN     = 8  //inches (1 inch = 2.54 centimeters).
	UT_CM     = 9  //centimeters.
	UT_MM     = 10 //millimeters.
	UT_PT     = 11 //points (1 point = 1/72 inches).
	UT_PC     = 12 //picas (1 pica = 12 points).
	UT_DIP    = 13
	reserved3 = 14
	UT_COLOR  = 15     // color in int
	UT_URL    = 16     // url in string
	UT_SYMBOL = 0xFFFF // for T_STRINGs designates symbol string ( so called NAME_TOKEN - CSS or JS identifier )
)

// enum VALUE_UNIT_TYPE_DATE
const (
	DT_HAS_DATE    = 0x01 // date contains date portion
	DT_HAS_TIME    = 0x02 // date contains time portion HH:MM
	DT_HAS_SECONDS = 0x04 // date contains time and seconds HH:MM:SS
	DT_UTC         = 0x10 // T_DATE is known to be UTC. Otherwise it is local date/time
)

// Sciter or TIScript specific
// enum VALUE_UNIT_TYPE_OBJECT
const (
	UT_OBJECT_ARRAY    = 0 // type T_OBJECT of type Array
	UT_OBJECT_OBJECT   = 1 // type T_OBJECT of type Object
	UT_OBJECT_CLASS    = 2 // type T_OBJECT of type Type (class or namespace)
	UT_OBJECT_NATIVE   = 3 // type T_OBJECT of native Type with data slot (LPVOID)
	UT_OBJECT_FUNCTION = 4 // type T_OBJECT of type Function
	UT_OBJECT_ERROR    = 5 // type T_OBJECT of type Error
)

// enum VALUE_UNIT_UNDEFINED
const (
	UT_NOTHING = 1
)

type ValueStringConvertType uint32

// enum VALUE_STRING_CVT_TYPE
const (
	CVT_SIMPLE        ValueStringConvertType = iota ///< simple conversion of terminal values
	CVT_JSON_LITERAL                                ///< json literal parsing/emission
	CVT_JSON_MAP                                    ///< json parsing/emission it parses as if token '{' already recognized
	CVT_XJSON_LITERAL                               ///< x-json parsing/emission, date is emitted as ISO8601 date literal, currency is emitted in the form DDDD$CCC
)

type SET_ELEMENT_HTML int32

// enum SET_ELEMENT_HTML
const (
	SIH_REPLACE_CONTENT   SET_ELEMENT_HTML = 0
	SIH_INSERT_AT_START   SET_ELEMENT_HTML = 1
	SIH_APPEND_AFTER_LAST SET_ELEMENT_HTML = 2
	SOH_REPLACE           SET_ELEMENT_HTML = 3
	SOH_INSERT_BEFORE     SET_ELEMENT_HTML = 4
	SOH_INSERT_AFTER      SET_ELEMENT_HTML = 5
)

type InitializationParams struct {
	Cmd uint32
}

type MouseParams struct {
	Cmd         MouseEvent // MouseEvents
	Target      C.HELEMENT
	Pos         Point
	DocumentPos Point
	ButtonState MouseButton
	AltState    KeyboardState
	CursorType  CursorType
	IsOnIcon    int32

	Dragging     C.HELEMENT
	DraggingMode DraggingType
}

type KeyParams struct {
	Cmd      KeyEvent // KeyEvents
	Target   C.HELEMENT
	KeyCode  uint32
	AltState KeyboardState
}

type FocusParams struct {
	Cmd          FocusEvent // FocusEvents
	Target       C.HELEMENT
	ByMouseClick int32 // boolean // true if focus is being set by mouse click
	Cancel       int32 // boolean // in FOCUS_LOST phase setting this field to true will cancel transfer focus from old element to the new one.
}

type DrawParams struct {
	Cmd      DrawEvent // DrawEvents
	Hdc      uintptr
	Area     Rect
	reserved uint32
}

type TimerParams struct {
	TimerId uintptr
}

// typedef struct BEHAVIOR_EVENT_PARAMS
// {
//   UINT     cmd;        // BEHAVIOR_EVENTS
//   HELEMENT heTarget;   // target element handler, in MENU_ITEM_CLICK this is owner element that caused this menu - e.g. context menu owner
//                        // In scripting this field named as Event.owner
//   HELEMENT he;         // source element e.g. in SELECTION_CHANGED it is new selected <option>, in MENU_ITEM_CLICK it is menu item (LI) element
//   UINT_PTR reason;     // EVENT_REASON or EDIT_CHANGED_REASON - UI action causing change.
//                        // In case of custom event notifications this may be any
//                        // application specific value.
//   SCITER_VALUE
//            data;       // auxiliary data accompanied with the event. E.g. FORM_SUBMIT event is using this field to pass collection of values.
// } BEHAVIOR_EVENT_PARAMS;
type BehaviorEventParams C.BEHAVIOR_EVENT_PARAMS

func (b *BehaviorEventParams) Cmd() BehaviorEvent {
	return BehaviorEvent(b.cmd & 0xFFF)
}

func (b *BehaviorEventParams) Phase() PhaseMask {
	return PhaseMask(b.cmd & 0xFFFFF000)
}

type MethodParams struct {
	MethodId BehaviorMethodIdentifier
	// Text     *uint16
	// Length   uint32
}

// typedef struct SCRIPTING_METHOD_PARAMS
// {
//     LPCSTR        name;   //< method name
//     SCITER_VALUE* argv;   //< vector of arguments
//     UINT          argc;   //< argument count
//     SCITER_VALUE  result; //< return value
// } SCRIPTING_METHOD_PARAMS;

type ScriptingMethodParams C.SCRIPTING_METHOD_PARAMS

func (s *ScriptingMethodParams) Name() string {
	return C.GoString(s.name)
}

func (s *ScriptingMethodParams) Argc() int {
	return int(s.argc)
}

func (r *ScriptingMethodParams) Args() []*Value {
	args := make([]*Value, 0)
	step := unsafe.Sizeof(r.result)
	for i := 0; i < int(r.Argc()); i++ {
		p := uintptr(unsafe.Pointer(r.argv)) + uintptr(i)*step
		args = append(args, (*Value)(unsafe.Pointer(p)))
	}
	return args
}

func (r *ScriptingMethodParams) Arg(i int) *Value {
	if i >= int(r.Argc()) {
		panic("ScriptingMethodParams arg index out of range")
	}
	p := uintptr(unsafe.Pointer(r.argv)) + uintptr(i)*unsafe.Sizeof(r.result)
	return (*Value)(unsafe.Pointer(p))
}

// set the return val for the scripting func
// if not set return undefined
func (s *ScriptingMethodParams) Return(val interface{}) {
	v := (*Value)(unsafe.Pointer(&(s.result)))
	v.init()
	v.Assign(val)
}

type PTiscriptVM uintptr
type HVM PTiscriptVM

// // tiscript_value
// typedef UINT64 tiscript_value;
type TiscriptValue uint64

// // pinned tiscript_value, val here will survive GC.
// typedef struct tiscript_pvalue
// {
//    tiscript_value val;
//    struct tiscript_VM* vm;
//    void *d1,*d2;
// } tiscript_pvalue;
type TiscriptPvalue struct {
	Val    TiscriptValue
	VM     HVM
	d1, d2 uintptr
}

// typedef struct TISCRIPT_METHOD_PARAMS
// {
//     tiscript_VM*   vm;
//     tiscript_value tag;    //< method id (symbol)
//     tiscript_value result; //< return value
//     // parameters are accessible through tiscript::args.
// } TISCRIPT_METHOD_PARAMS;
type TiscriptMethodParams struct {
	// tiscript_VM    *vm
	VM HVM
	// tiscript_value tag    //< method id (symbol)
	Tag TiscriptValue
	// tiscript_value result //< return value
	Result TiscriptValue
}

// typedef struct DATA_ARRIVED_PARAMS
// {
//     HELEMENT  initiator;    // element intiator of HTMLayoutRequestElementData request,
//     LPCBYTE   data;         // data buffer
//     UINT      dataSize;     // size of data
//     UINT      dataType;     // data type passed "as is" from HTMLayoutRequestElementData
//     UINT      status;       // status = 0 (dataSize == 0) - unknown error.
//                             // status = 100..505 - http response status, Note: 200 - OK!
//                             // status > 12000 - wininet error code, see ERROR_INTERNET_*** in wininet.h
//     LPCWSTR   uri;          // requested url
// } DATA_ARRIVED_PARAMS;
type DataArrivedParams C.DATA_ARRIVED_PARAMS

// type DataArrivedParams struct {
// 	Initiator C.HELEMENT
// 	data      *byte
// 	DataSize  uint32
// 	DataType  uint32
// 	Status    uint32
// 	uri       *uint16 // Wide character string
// }

func (d *DataArrivedParams) Uri() string {
	return Utf16ToString((*uint16)(unsafe.Pointer(d.uri)))
}

func (d *DataArrivedParams) Data() []byte {
	return BytePtrToBytes((*byte)(unsafe.Pointer(d.data)), uint(d.dataSize))
}

// struct SCROLL_PARAMS
// {
//   UINT      cmd;          // SCROLL_EVENTS
//   HELEMENT  target;       // target element
//   INT       pos;          // scroll position if SCROLL_POS
//   BOOL      vertical;     // true if from vertical scrollbar
// };
type ScrollParams struct {
	Cmd      ScrollEvent
	Target   C.HELEMENT
	Pos      int32
	Vertical int32 // bool
}

// enum EXCHANGE_CMD {
// 	X_DRAG_ENTER = 0,       // drag enters the element
// 	X_DRAG_LEAVE = 1,       // drag leaves the element
// 	X_DRAG = 2,             // drag over the element
// 	X_DROP = 3,             // data dropped on the element
// 	X_PASTE = 4,            // N/A
// 	X_DRAG_REQUEST = 5,     // N/A
// 	X_DRAG_CANCEL = 6,      // drag cancelled (e.g. by pressing VK_ESCAPE)
// 	X_WILL_ACCEPT_DROP = 7, // drop target element shall consume this event in order to receive X_DROP
// };
type ExchangeCmd uint32

const (
	X_DRAG_ENTER ExchangeCmd = iota
	X_DRAG_LEAVE
	X_DRAG
	X_DROP
	X_PASTE
	X_DRAG_REQUEST
	X_DRAG_CANCEL
	X_WILL_ACCEPT_DROP
)

// enum DD_MODES {
// 	DD_MODE_NONE = 0, // DROPEFFECT_NONE	( 0 )
// 	DD_MODE_COPY = 1, // DROPEFFECT_COPY	( 1 )
// 	DD_MODE_MOVE = 2, // DROPEFFECT_MOVE	( 2 )
// 	DD_MODE_COPY_OR_MOVE = 3, // DROPEFFECT_COPY	( 1 ) | DROPEFFECT_MOVE	( 2 )
// 	DD_MODE_LINK = 4, // DROPEFFECT_LINK	( 4 )
// };
type DDMode uint32

const (
	DD_MODE_NONE DDMode = 0
	DD_MODE_COPY
	DD_MODE_MOVE
	DD_MODE_COPY_OR_MOVE
	DD_MODE_LINK
)

// struct EXCHANGE_PARAMS
// {
//   UINT         cmd;          // EXCHANGE_EVENTS
//   HELEMENT     target;       // target element
//   HELEMENT     source;       // source element (can be null if D&D from external window)
//   POINT        pos;          // position of cursor, element relative
//   POINT        pos_view;     // position of cursor, view relative
//   UINT         mode;         // DD_MODE
//   SCITER_VALUE data;         // packaged drag data
// };
type ExchangeParams struct {
	Cmd     ExchangeCmd
	Target  C.HELEMENT
	Source  C.HELEMENT
	Pos     Point
	PosView Point
	Mode    DDMode
	Data    Value
}

// struct GESTURE_PARAMS
// {
// UINT      cmd;          // GESTURE_EVENTS
// HELEMENT  target;       // target element
// POINT     pos;          // position of cursor, element relative
// POINT     pos_view;     // position of cursor, view relative
// UINT      flags;        // for GESTURE_REQUEST combination of GESTURE_FLAGs.
//                         // for others it is a combination of GESTURE_STATe's
// UINT      delta_time;   // period of time from previous event.
// SIZE      delta_xy;     // for GESTURE_PAN it is a direction vector
// double    delta_v;      // for GESTURE_ROTATE - delta angle (radians)
//                         // for GESTURE_ZOOM - zoom value, is less or greater than 1.0
// };
type GestureParams struct {
	Cmd       GestureCmd
	Target    C.HELEMENT
	Pos       Point
	PosView   Point
	Flags     uint32
	DeltaTime uint32
	DeltaXY   Size
	DeltaV    float64
}

type SomEvents uint32

const (
	SOM_GET_PASSPORT = 0
	SOM_GET_ASSET    = 1
)

type SomParams C.SOM_PARAMS

const (
	LOAD_OK      = 0 // do default loading if data not set
	LOAD_DISCARD = 1 // discard request completely
	LOAD_DELAYED = 2 // data will be delivered later by the host application.
)

const (
	/**Notifies that Sciter is about to download a referred resource.
	 *
	 * \param lParam #LPSCN_LOAD_DATA.
	 * \return #LOAD_OK or #LOAD_DISCARD
	 *
	 * This notification gives application a chance to override built-in loader and
	 * implement loading of resources in its own way (for example images can be loaded from
	 * database or other resource). To do this set #SCN_LOAD_DATA::outData and
	 * #SCN_LOAD_DATA::outDataSize members of SCN_LOAD_DATA. Sciter does not
	 * store pointer to this data. You can call #SciterDataReady() function instead
	 * of filling these fields. This allows you to free your outData buffer
	 * immediately.
	**/
	SC_LOAD_DATA = 0x01

	/**This notification indicates that external data (for example image) download process
	 * completed.
	 *
	 * \param lParam #LPSCN_DATA_LOADED
	 *
	 * This notifiaction is sent for each external resource used by document when
	 * this resource has been completely downloaded. Sciter will send this
	 * notification asynchronously.
	 **/
	SC_DATA_LOADED = 0x02

	/**This notification is sent when all external data (for example image) has been downloaded.
	 *
	 * This notification is sent when all external resources required by document
	 * have been completely downloaded. Sciter will send this notification
	 * asynchronously.
	 **/
	/* obsolete #define SC_DOCUMENT_COMPLETE 0x03
	use DOCUMENT_COMPLETE DOM event.
	*/

	/**This notification is sent on parsing the document and while processing
	 * elements having non empty style.behavior attribute value.
	 *
	 * \param lParam #LPSCN_ATTACH_BEHAVIOR
	 *
	 * Application has to provide implementation of #sciter::behavior interface.
	 * Set #SCN_ATTACH_BEHAVIOR::impl to address of this implementation.
	 **/
	SC_ATTACH_BEHAVIOR = 0x04

	/**This notification is sent when instance of the engine is destroyed.
	 * It is always final notification.
	 *
	 * \param lParam #LPSCN_ENGINE_DESTROYED
	 *
	 **/
	SC_ENGINE_DESTROYED = 0x05

	/**Posted notification.

	 * \param lParam #LPSCN_POSTED_NOTIFICATION
	 *
	 **/
	SC_POSTED_NOTIFICATION = 0x06

	/**This notification is sent when the engine encounters critical rendering error: e.g. DirectX gfx driver error.
	  Most probably bad gfx drivers.

	* \param lParam #LPSCN_GRAPHICS_CRITICAL_FAILURE
	*
	**/
	SC_GRAPHICS_CRITICAL_FAILURE = 0x07

	/**This notification is sent when the engine needs keyboard to be present on screen
	  E.g. when <input|text> gets focus

	* \param lParam #LPSCN_KEYBOARD_REQUEST
	*
	**/
	SC_KEYBOARD_REQUEST = 0x08

	/**This notification is sent when the engine needs some area to be redrawn

	 * \param lParam #LPSCN_INVLIDATE_RECT
	 *
	 **/
	SC_INVALIDATE_RECT = 0x09
)

// Notify structures
/**Notification callback structure.
 **/
// typedef struct SCITER_CALLBACK_NOTIFICATION
// {
//   UINT code; /**< [in] one of the codes above.*/
//   HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
// } SCITER_CALLBACK_NOTIFICATION;
type SciterCallbackNotification struct {
	Code uint32
	Hwnd C.HWINDOW
}

/**This structure is used by #SC_ENGINE_DESTROYED notification.
 *\copydoc SCN_ENGINE_DESTROYED **/
// typedef struct SCN_ENGINE_DESTROYED
// {
//     UINT code; /**< [in] one of the codes above.*/
//     HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
// } SCN_ENGINE_DESTROYED;
type ScnEngineDestroyed SciterCallbackNotification

/**This structure is used by #SC_ENGINE_DESTROYED notification.
 *\copydoc SCN_ENGINE_DESTROYED **/
//typedef struct SCN_POSTED_NOTIFICATION
//{
//    UINT      code; /**< [in] one of the codes above.*/
//    HWINDOW   hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
//    UINT_PTR  wparam;
//    UINT_PTR  lparam;
//    UINT_PTR  lreturn;
//} SCN_POSTED_NOTIFICATION;
type ScnPostedNotification struct {
	SciterCallbackNotification
	Wparam  *uint
	Lparam  *uint
	Lreturn *uint
}

/**This structure is used by #SC_GRAPHICS_CRITICAL_FAILURE notification.
 *\copydoc SC_GRAPHICS_CRITICAL_FAILURE **/
type ScnGraphicsCriticalFailure SciterCallbackNotification

/**This structure is used by #SC_KEYBOARD_REQUEST notification.
 *\copydoc SC_KEYBOARD_REQUEST **/
type ScnKeyboardRequest struct {
	SciterCallbackNotification
	// 0 - hide keyboard, 1 ... - type of keyboard
	keyboardMode uint
}

/**This structure is used by #SC_INVALIDATE_RECT notification.
 *\copydoc SC_INVALIDATE_RECT **/
type ScnInvalidateRect struct {
	SciterCallbackNotification
	// cumulative invalid rect
	invalidRect Rect
}

/**This structure is used by #SCN_LOAD_DATA notification.
 *\copydoc SCN_LOAD_DATA
 **/

//typedef struct SCN_LOAD_DATA
//{
//    UINT code; /**< [in] one of the codes above.*/
//    HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
//
//    LPCWSTR  uri;              /**< [in] Zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/
//
//    LPCBYTE  outData;          /**< [in,out] pointer to loaded data to return. if data exists in the cache then this field contain pointer to it*/
//    UINT     outDataSize;      /**< [in,out] loaded data size to return.*/
//    UINT     dataType;         /**< [in] SciterResourceType */
//
//    LPVOID   requestId;        /**< [in] request id that needs to be passed as is to the SciterDataReadyAsync call */
//
//    HELEMENT principal;
//    HELEMENT initiator;
//} SCN_LOAD_DATA;

type ScnLoadData C.SCN_LOAD_DATA

// type ScnLoadData struct {
// 	SciterCallbackNotification
// 	uri         *uint16
// 	outData     *byte
// 	outDataSize uint
// 	DataType    uint
// 	RequestId   uintptr
// 	Principal   C.HELEMENT
// 	Initiator   C.HELEMENT
// }

func (s *ScnLoadData) Uri() string {
	return Utf16ToString((*uint16)(unsafe.Pointer(s.uri)))
}

func (s *ScnLoadData) RequestId() C.HREQUEST {
	return s.requestId
}

func (s *ScnLoadData) Data() []byte {
	ret := ByteCPtrToBytes(s.outData, s.outDataSize)
	return ret
}

func (s *ScnLoadData) SetData(data []byte) {
	s.outData = (C.LPCBYTE)(unsafe.Pointer((&data[0])))
	s.outDataSize = C.UINT(len(data))
}

/**This structure is used by #SCN_DATA_LOADED notification.
 *\copydoc SCN_DATA_LOADED
 **/
//typedef struct SCN_DATA_LOADED
//{
//    UINT code; /**< [in] one of the codes above.*/
//    HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
//
//    LPCWSTR  uri;              /**< [in] zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/
//    LPCBYTE  data;             /**< [in] pointer to loaded data.*/
//    UINT    dataSize;         /**< [in] loaded data size (in bytes).*/
//    UINT     dataType;         /**< [in] SciterResourceType */
//    UINT     status;           /**< [in]
//                                         status = 0 (dataSize == 0) - unknown error.
//                                         status = 100..505 - http response status, Note: 200 - OK!
//                                         status > 12000 - wininet error code, see ERROR_INTERNET_*** in wininet.h
//                                 */
//} SCN_DATA_LOADED;
type ScnDataLoaded struct {
	SciterCallbackNotification
	uri      *uint16
	data     *byte
	DataSize uint32
	DataType SciterResourceType
	Status   uint32
}

func (s *ScnDataLoaded) Uri() string {
	return Utf16ToString(s.uri)
}

func (s *ScnDataLoaded) Data() []byte {
	return BytePtrToBytes(s.data, uint(s.DataSize))
}

//typedef struct SCN_ATTACH_BEHAVIOR
//{
//    UINT code; /**< [in] one of the codes above.*/
//    HWINDOW hwnd; /**< [in] HWINDOW of the window this callback was attached to.*/
//
//    HELEMENT element;          /**< [in] target DOM element handle*/
//    LPCSTR   behaviorName;     /**< [in] zero terminated string, string appears as value of CSS behavior:"???" attribute.*/
//
//    ElementEventProc* elementProc;    /**< [out] pointer to ElementEventProc function.*/
//    LPVOID            elementTag;     /**< [out] tag value, passed as is into pointer ElementEventProc function.*/
//
//} SCN_ATTACH_BEHAVIOR;
type ScnAttachBehavior C.SCN_ATTACH_BEHAVIOR

// type ScnAttachBehavior struct {
// 	SciterCallbackNotification
// 	Element      C.HELEMENT
// 	behaviorName *byte
// 	ElementProc  uintptr
// 	ElementTag   uintptr
// }

func (s *ScnAttachBehavior) BehaviorName() string {
	return C.GoString(s.behaviorName)
}

func (s *ScnAttachBehavior) Element() C.HELEMENT {
	return s.element
}

// typedef int SCDOM_RESULT;
type SCDOM_RESULT int32
type VALUE_RESULT int32 //C.VALUE_RESULT

// enum VALUE_RESULT
const (
	HV_OK_TRUE VALUE_RESULT = iota - 1
	HV_OK
	HV_BAD_PARAMETER
	HV_INCOMPATIBLE_TYPE
)

const (
	SCDOM_OK SCDOM_RESULT = iota
	SCDOM_INVALID_HWND
	SCDOM_INVALID_HANDLE
	SCDOM_PASSIVE_HANDLE
	SCDOM_INVALID_PARAMETER
	SCDOM_OPERATION_FAILED
	SCDOM_OK_NOT_HANDLED SCDOM_RESULT = (-1)
)

type domError struct {
	Result  SCDOM_RESULT
	Message string
}

func (e *domError) Error() string {
	return fmt.Sprintf("%s: %s", e.Result.String(), e.Message)
}

func newDomError(ret SCDOM_RESULT, msg string) *domError {
	return &domError{
		Result:  ret,
		Message: msg,
	}
}

// return nil when r == SCDOM_OK
func wrapDomResult(r C.INT, msg string) error {
	if r == C.INT(SCDOM_OK) {
		return nil
	}
	return newDomError(SCDOM_RESULT(r), msg)
}

// enum ELEMENT_AREAS
const (
	ROOT_RELATIVE = 0x01 // - or this flag if you want to get HTMLayout window relative coordinates
	//   otherwise it will use nearest windowed container e.g. popup window.
	SELF_RELATIVE = 0x02 // - "or" this flag if you want to get coordinates relative to the origin
	//   of element iself.
	CONTAINER_RELATIVE = 0x03 // - position inside immediate container.
	VIEW_RELATIVE      = 0x04 // - position relative to view - HTMLayout window

	CONTENT_BOX = 0x00 // content (inner)  box
	PADDING_BOX = 0x10 // content + paddings
	BORDER_BOX  = 0x20 // content + paddings + border
	MARGIN_BOX  = 0x30 // content + paddings + border + margins

	BACK_IMAGE_AREA = 0x40 // relative to content origin - location of background image (if it set no-repeat)
	FORE_IMAGE_AREA = 0x50 // relative to content origin - location of foreground image (if it set no-repeat)

	SCROLLABLE_AREA = 0x60 // scroll_area - scrollable area in content box
)

type SCITER_SCROLL_FLAGS uint32

// enum SCITER_SCROLL_FLAGS
const (
	SCROLL_TO_TOP SCITER_SCROLL_FLAGS = 0x01
	SCROLL_SMOOTH SCITER_SCROLL_FLAGS = 0x10
)

type Point struct {
	X int32
	Y int32
}

type Size struct {
	Cx int32
	Cy int32
}

type Rect struct {
	Left   int32
	Top    int32
	Right  int32
	Bottom int32
}

func NewRect(top, left, width, height int) *Rect {
	return &Rect{
		Top:    int32(top),
		Left:   int32(left),
		Bottom: int32(top + height),
		Right:  int32(left + width),
	}
}

var (
	DefaultRect = &Rect{0, 0, 300, 400}
)

type EventHandler struct {
	OnAttached func(he *Element)
	OnDetached func(he *Element)
	OnMouse    func(he *Element, params *MouseParams) bool
	OnKey      func(he *Element, params *KeyParams) bool
	OnFocus    func(he *Element, params *FocusParams) bool
	OnDraw     func(he *Element, params *DrawParams) bool
	OnTimer    func(he *Element, params *TimerParams) bool
	// notification events from builtin behaviors - synthesized events: BUTTON_CLICK, VALUE_CHANGED
	// see enum BEHAVIOR_EVENTS
	OnBehaviorEvent func(he *Element, params *BehaviorEventParams) bool
	OnMethodCall    func(he *Element, params *MethodParams) bool
	// calls from CSSS! script and TIScript (if it was not handled by method below).

	// If needed your application may expose some [native] functions to be called by script code.
	// Usually this is made by implementing your own event_handler and overriding
	// its on_script_call(helement, name, argc, argv, retval) method.
	// If you will do this then you can invoke this callback from script as:
	//     "global" native functions: var r = view.funcName( p0, p1, ... );
	//              - calling  on_script_call of event_handler instance attached to the window.
	//     As element's methods: var r = el.funcName( p0, p1, ... );
	//              - calling  on_script_call of event_handler instance (native behavior) attached to the element.
	// This way you can establish interaction between scipt and native code inside your application.
	OnScriptingMethodCall func(he *Element, params *ScriptingMethodParams) bool
	// Calls from TIScript. Override this if you want your own methods accessible directly from tiscript engine.
	// Use tiscript::args to access parameters.
	OnTiscriptMethodCall func(he *Element, params *TiscriptMethodParams) bool
	// call when resource is loaded but not used
	// return true would cancel the resource usage
	// return false would follow the normal procedure
	OnDataArrived func(he *Element, params *DataArrivedParams) bool
	OnSize        func(he *Element)
	OnScroll      func(he *Element, params *ScrollParams) bool
	OnExchange    func(he *Element, params *ExchangeParams) bool
	OnGesture     func(he *Element, params *GestureParams) bool
	OnSom         func(he *Element, params *SomParams) bool
}

// case SC_LOAD_DATA:          return static_cast<BASE*>(this)->on_load_data((LPSCN_LOAD_DATA) pnm);
// case SC_DATA_LOADED:        return static_cast<BASE*>(this)->on_data_loaded((LPSCN_DATA_LOADED)pnm);
// //case SC_DOCUMENT_COMPLETE: return on_document_complete();
// case SC_ATTACH_BEHAVIOR:    return static_cast<BASE*>(this)->on_attach_behavior((LPSCN_ATTACH_BEHAVIOR)pnm );
// case SC_ENGINE_DESTROYED:   return static_cast<BASE*>(this)->on_engine_destroyed();
// case SC_POSTED_NOTIFICATION: return static_cast<BASE*>(this)->on_posted_notification((LPSCN_POSTED_NOTIFICATION)pnm);

type CallbackHandler struct {
	Behaviors map[string]*EventHandler
	// Notifies that Sciter is about to download a referred resource.
	OnLoadData func(params *ScnLoadData) int
	// This notification indicates that external data (for example image) download process
	OnDataLoaded func(params *ScnDataLoaded) int
	/**This notification is sent on parsing the document and while processing
	 * elements having non empty style.behavior attribute value.
	 *
	 * \param lParam #LPSCN_ATTACH_BEHAVIOR
	 *
	 * Application has to provide implementation of #sciter::behavior interface.
	 * Set #SCN_ATTACH_BEHAVIOR::impl to address of this implementation.
	 **/
	OnAttachBehavior func(params *ScnAttachBehavior) int
	/**This notification is sent when instance of the engine is destroyed.
	 * It is always final notification.
	 *
	 * \param lParam #LPSCN_ENGINE_DESTROYED
	 *
	 **/
	OnEngineDestroyed func() int
	/**Posted notification.

	 * \param lParam #LPSCN_POSTED_NOTIFICATION
	 *
	 **/
	OnPostedNotification func(params *ScnPostedNotification) int

	/**This notification is sent when the engine encounters critical rendering error: e.g. DirectX gfx driver error.
	  Most probably bad gfx drivers.

	* \param lParam #LPSCN_GRAPHICS_CRITICAL_FAILURE
	*
	**/
	OnGraphicsCriticalFailure func() int

	/**This notification is sent when the engine needs keyboard to be present on screen
	  E.g. when <input|text> gets focus

	* \param lParam #LPSCN_KEYBOARD_REQUEST
	*
	**/
	OnKeyboardRequest func(params *ScnKeyboardRequest) int

	/**This notification is sent when the engine needs some area to be redrawn

	 * \param lParam #LPSCN_INVLIDATE_RECT
	 *
	 **/
	OnInvalidateRect func(params *ScnInvalidateRect) int
}

// enum SCRIPT_RUNTIME_FEATURES
// {
const (
	ALLOW_FILE_IO   = 0x00000001
	ALLOW_SOCKET_IO = 0x00000002
	ALLOW_EVAL      = 0x00000004
	ALLOW_SYSINFO   = 0x00000008
)

// };

// enum GFX_LAYER
// {
const (
	GFX_LAYER_GDI         = 1
	GFX_LAYER_CG          = 1
	GFX_LAYER_CAIRO       = 1
	GFX_LAYER_WARP        = 2
	GFX_LAYER_D2D         = 3
	GFX_LAYER_SKIA        = 4
	GFX_LAYER_SKIA_OPENGL = 5
	GFX_LAYER_AUTO        = 0xFFFF
)

// };

type Sciter_RT_OPTIONS uint32

const (
	SCITER_SMOOTH_SCROLL      Sciter_RT_OPTIONS = 1 // value:TRUE - enable, value:FALSE - disable, enabled by default
	SCITER_CONNECTION_TIMEOUT                   = 2 // value: milliseconds, connection timeout of http client
	SCITER_HTTPS_ERROR                          = 3 // value: 0 - drop connection, 1 - use builtin dialog, 2 - accept connection silently
	SCITER_FONT_SMOOTHING                       = 4 // value: 0 - system default, 1 - no smoothing, 2 - std smoothing, 3 - clear type
	_
	SCITER_TRANSPARENT_WINDOW = 6 // Windows Aero support, value:
	// 0 - normal drawing,
	// 1 - window has transparent background after calls DwmExtendFrameIntoClientArea() or DwmEnableBlurBehindWindow().
	SCITER_SET_GPU_BLACKLIST = 7 // hWnd = NULL,
	// value = LPCBYTE, json - GPU black list, see: gpu-blacklist.json resource.
	SCITER_SET_SCRIPT_RUNTIME_FEATURES = 8  // value - combination of SCRIPT_RUNTIME_FEATURES flags.
	SCITER_SET_GFX_LAYER               = 9  // hWnd = NULL, value - GFX_LAYER
	SCITER_SET_DEBUG_MODE              = 10 // hWnd, value - TRUE/FALSE
	SCITER_SET_UX_THEMING              = 11 // hWnd = NULL, value - BOOL, TRUE - the engine will use "unisex" theme that is common for all platforms.
	// That UX theme is not using OS primitives for rendering input elements. Use it if you want exactly
	// the same (modulo fonts) look-n-feel on all platforms.

	SCITER_ALPHA_WINDOW = 12 //  hWnd, value - TRUE/FALSE - window uses per pixel alpha (e.g. WS_EX_LAYERED/UpdateLayeredWindow() window)

	SCITER_SET_INIT_SCRIPT = 13 // hWnd - N/A , value LPCSTR - UTF-8 encoded script source to be loaded into each view before any other script execution.
	//                             The engine copies this string inside the call.

	SCITER_SET_MAIN_WINDOW = 14 //  hWnd, value - TRUE/FALSE - window is main, will destroy all other dependent windows on close

	SCITER_SET_MAX_HTTP_DATA_LENGTH = 15 // hWnd - N/A , value - max request length in megabytes (1024*1024 bytes)
)

// * \param[in] placement \b UINT, values:
// *     2 - popup element below of anchor
// *     8 - popup element above of anchor
// *     4 - popup element on left side of anchor
// *     6 - popup element on right side of anchor
// *     ( see numpad on keyboard to get an idea of the numbers)
type PopupPlacement uint32

const (
	BelowAnchor   PopupPlacement = 2
	aboveAnchor   PopupPlacement = 8
	LeftOfAnchor  PopupPlacement = 4
	RightOfAnchor PopupPlacement = 6
)

type ElementState uint32

// enum ELEMENT_STATE_BITS
const (
	STATE_LINK       ElementState = 0x00000001
	STATE_HOVER      ElementState = 0x00000002
	STATE_ACTIVE     ElementState = 0x00000004
	STATE_FOCUS      ElementState = 0x00000008
	STATE_VISITED    ElementState = 0x00000010
	STATE_CURRENT    ElementState = 0x00000020 // current (hot) item
	STATE_CHECKED    ElementState = 0x00000040 // element is checked (or selected)
	STATE_DISABLED   ElementState = 0x00000080 // element is disabled
	STATE_READONLY   ElementState = 0x00000100 // readonly input element
	STATE_EXPANDED   ElementState = 0x00000200 // expanded state - nodes in tree view
	STATE_COLLAPSED  ElementState = 0x00000400 // collapsed state - nodes in tree view - mutually exclusive with
	STATE_INCOMPLETE ElementState = 0x00000800 // one of fore/back images requested but not delivered
	STATE_ANIMATING  ElementState = 0x00001000 // is animating currently
	STATE_FOCUSABLE  ElementState = 0x00002000 // will accept focus
	STATE_ANCHOR     ElementState = 0x00004000 // anchor in selection (used with current in selects)
	STATE_SYNTHETIC  ElementState = 0x00008000 // this is a synthetic element - don't emit it's head/tail
	STATE_OWNS_POPUP ElementState = 0x00010000 // this is a synthetic element - don't emit it's head/tail
	STATE_TABFOCUS   ElementState = 0x00020000 // focus gained by tab traversal
	STATE_EMPTY      ElementState = 0x00040000 // empty - element is empty (text.size() == 0 && subs.size() == 0)
	//  if element has behavior attached then the behavior is responsible for the value of this flag.
	STATE_BUSY ElementState = 0x00080000 // busy; loading

	STATE_DRAG_OVER   ElementState = 0x00100000 // drag over the block that can accept it (so is current drop target). Flag is set for the drop target block
	STATE_DROP_TARGET ElementState = 0x00200000 // active drop target.
	STATE_MOVING      ElementState = 0x00400000 // dragging/moving - the flag is set for the moving block.
	STATE_COPYING     ElementState = 0x00800000 // dragging/copying - the flag is set for the copying block.
	STATE_DRAG_SOURCE ElementState = 0x01000000 // element that is a drag source.
	STATE_DROP_MARKER ElementState = 0x02000000 // element is drop marker

	STATE_PRESSED ElementState = 0x04000000 // pressed - close to active but has wider life span - e.g. in MOUSE_UP it
	//   is still on; so behavior can check it in MOUSE_UP to discover CLICK condition.
	STATE_POPUP ElementState = 0x08000000 // this element is out of flow - popup

	STATE_IS_LTR ElementState = 0x10000000 // the element or one of its containers has dir=ltr declared
	STATE_IS_RTL ElementState = 0x20000000 // the element or one of its containers has dir=rtl declared
)

type RequestType uint32

// enum REQUEST_TYPE
const (
	GET_ASYNC  RequestType = iota // async GET
	POST_ASYNC                    // async POST
	GET_SYNC                      // synchronous GET
	POST_SYNC                     // synchronous POST
)

// struct REQUEST_PARAM {
//   LPCWSTR name;
//   LPCWSTR value;
// };

type RequestParam struct {
	Name  string
	Value string
}
