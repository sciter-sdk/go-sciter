package sciter

type eventMapper struct {
	mouseHandlerList         []func(he *Element, params *MouseParams) bool
	keyHandlerList           []func(he *Element, params *KeyParams) bool
	focusHandlerList         []func(he *Element, params *FocusParams) bool
	timerHandlerList         []func(he *Element, params *TimerParams) bool
	behaviorEventHandlerList []func(he *Element, params *BehaviorEventParams) bool
	methodCallHandlerList    []func(he *Element, params *MethodParams) bool
	scriptingMethodMap       map[string]func(args ...*Value) *Value
	dataArrivedHandlerList   []func(he *Element, params *DataArrivedParams) bool
	sizeHandlerList          []func(he *Element)
	scrollHandlerList        []func(he *Element, params *ScrollParams) bool
	gestureHandlerList       []func(he *Element, params *GestureParams) bool
	// the handler instance
	eventhandler *EventHandler
}

func newEventMapper() *eventMapper {
	em := &eventMapper{
		mouseHandlerList:         make([]func(he *Element, params *MouseParams) bool, 0),
		keyHandlerList:           make([]func(he *Element, params *KeyParams) bool, 0),
		focusHandlerList:         make([]func(he *Element, params *FocusParams) bool, 0),
		timerHandlerList:         make([]func(he *Element, params *TimerParams) bool, 0),
		behaviorEventHandlerList: make([]func(he *Element, params *BehaviorEventParams) bool, 0),
		methodCallHandlerList:    make([]func(he *Element, params *MethodParams) bool, 0),
		scriptingMethodMap:       make(map[string]func(args ...*Value) *Value),
		dataArrivedHandlerList:   make([]func(he *Element, params *DataArrivedParams) bool, 0),
		sizeHandlerList:          make([]func(he *Element), 0),
		scrollHandlerList:        make([]func(he *Element, params *ScrollParams) bool, 0),
		gestureHandlerList:       make([]func(he *Element, params *GestureParams) bool, 0),
	}
	em.eventhandler = em.toEventHandler()
	return em
}

func (s *Sciter) checkMapper() {
	if s.eventMapper == nil {
		s.eventMapper = newEventMapper()
		s.AttachWindowEventHandler(s.eventMapper.eventhandler)
	}
}

func (el *Element) checkMapper() {
	if el.eventMapper == nil {
		el.eventMapper = newEventMapper()
		el.AttachEventHandler(el.eventMapper.eventhandler)
	}
}

func (e *eventMapper) addMethod(name string, nf func(args ...*Value) *Value) {
	e.scriptingMethodMap[name] = nf
}

func (e *eventMapper) onClick(fn func()) {
	e.behaviorEventHandlerList = append(e.behaviorEventHandlerList, func(he *Element, params *BehaviorEventParams) bool {
		if params.Cmd() == BUTTON_CLICK && params.Phase() == SINKING {
			fn()
		}
		return false
	})
}

// DefineFunction defines global scripting function which works for tiscript and csss! script.
//   The global function must be access via the `view` namespace
func (s *Sciter) DefineFunction(name string, nf func(args ...*Value) *Value) {
	s.checkMapper()
	s.eventMapper.scriptingMethodMap[name] = nf
}

// DefineMethod defines Element locally scripting function in tiscript
//   The local method must be access as the element attribute.
func (e *Element) DefineMethod(name string, nf func(args ...*Value) *Value) {
	e.checkMapper()
	e.eventMapper.scriptingMethodMap[name] = nf
}

func (e *Element) OnClick(fn func()) {
	e.checkMapper()
	e.eventMapper.onClick(fn)
}

func (e *eventMapper) toEventHandler() *EventHandler {
	return &EventHandler{
		OnMouse: func(he *Element, params *MouseParams) bool {
			for _, h := range e.mouseHandlerList {
				h(he, params)
			}
			return false
		},
		OnKey: func(he *Element, params *KeyParams) bool {
			for _, h := range e.keyHandlerList {
				h(he, params)
			}
			return false
		},
		OnFocus: func(he *Element, params *FocusParams) bool {
			for _, h := range e.focusHandlerList {
				h(he, params)
			}
			return false
		},
		OnTimer: func(he *Element, params *TimerParams) bool {
			for _, h := range e.timerHandlerList {
				h(he, params)
			}
			return false
		},
		OnBehaviorEvent: func(he *Element, params *BehaviorEventParams) bool {
			for _, h := range e.behaviorEventHandlerList {
				h(he, params)
			}
			return false
		},
		OnMethodCall: func(he *Element, params *MethodParams) bool {
			for _, h := range e.methodCallHandlerList {
				h(he, params)
			}
			return false
		},
		OnScriptingMethodCall: func(he *Element, params *ScriptingMethodParams) bool {
			if nf, ok := e.scriptingMethodMap[params.Name()]; ok {
				params.Return(nf(params.Args()...))
				return true
			}
			return false
		},
		OnDataArrived: func(he *Element, params *DataArrivedParams) bool {
			for _, h := range e.dataArrivedHandlerList {
				h(he, params)
			}
			return false
		},
		OnSize: func(he *Element) {
			for _, h := range e.sizeHandlerList {
				h(he)
			}
		},
		OnScroll: func(he *Element, params *ScrollParams) bool {
			for _, h := range e.scrollHandlerList {
				h(he, params)
			}
			return false
		},
		OnGesture: func(he *Element, params *GestureParams) bool {
			for _, h := range e.gestureHandlerList {
				h(he, params)
			}
			return false
		},
	}
}
