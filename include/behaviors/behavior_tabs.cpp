#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"

namespace sciter 
{
/*

BEHAVIOR: tabs
   goal: Implementation of the tabs: stacked panels switchable by tabs in tab strip
COMMENTS: 
   <div style="behavior:tabs">
      <ul> <!-- our tab strip, can be placed on any side of tab container. -->
         <li panel="panel-id1" selected >tab1 caption</li>
         <li panel="panel-id2">tab2 caption</li>
      </ul>
      <div name="panel-id1" > first panel content </div>
      <div name="panel-id2"> second panel content </div>
   </div>
SAMPLE:
   See: samples/behaviors/tabs.htm
*/

  struct tabs : public event_handler
  {
    HELEMENT self = 0;
    // ctor
    tabs() : event_handler() {   }

    virtual bool subscription(HELEMENT he, UINT& event_groups)
    {
      event_groups = HANDLE_MOUSE |
        HANDLE_KEY |
        HANDLE_FOCUS |
        HANDLE_BEHAVIOR_EVENT |
        HANDLE_TIMER;
      return true;
    }

    virtual void attached(HELEMENT he)
    {
      self = he;
      dom::element tabs_el = he;              //:root below matches the element we use to start lookup from.
      dom::element tab_el = tabs_el.find_first(":root>.strip>[panel][selected]"); // initialy selected

      sciter::string pname = tab_el.get_attribute("panel");
      // find panel we need to show by default 
      dom::element panel_el = tabs_el.find_first(":root>[name=\"%s\"],:root>[id=\"%s\"]", aux::w2a(pname).c_str(), aux::w2a(pname).c_str());
      if (!panel_el.is_valid())
      {
        assert(false); // what a ...!, panel="somename" without matching name="somename"
        return;
      }

      dom::element tab_strip_el = tab_el.parent();
      tab_strip_el.set_state(STATE_CURRENT, 0); // :current - make tab strip as current element inside focusable.
      tab_el.set_state(STATE_CURRENT, 0); // :current - current tab is, well, current.
      panel_el.set_state(STATE_EXPANDED, 0); // :expanded - current panel is expanded.

    }

    virtual void detached(HELEMENT he)
    {
      asset_release();
    }

    virtual bool on_mouse(HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates)
    {
      if (event_type != MOUSE_DOWN && event_type != MOUSE_DCLICK /*&& event_type != DROP*/)
        return false;

      dom::element tabs_el = he; // our tabs container
      dom::element tab_el = target_tab(target, he);

      if (!tab_el.is_valid())
        return false;

      //el here is a <element panel="panel-id1">tab1 caption</element>
      //and we've got here MOUSE_DOWN somewhere on it.

      return select_tab(tabs_el, tab_el);
    }

    static inline bool is_in_focus(const dom::element& el)
    {
      return el.test(":focus") || el.find_nearest_parent(":focus");
    }

#ifdef WINDOWS
    virtual bool on_key(HELEMENT he, HELEMENT target, UINT event_type, UINT code, UINT keyboardStates)
    {
      if (event_type != KEY_DOWN)
        return FALSE; // we are handling only KEY_DOWN here

      dom::element tabs_el = he; // our tabs container
      dom::element tab_el = tabs_el.find_first(":root>.strip>[panel]:current"); // currently selected

      switch (code)
      {
      case VK_TAB: if (keyboardStates & CONTROL_KEY_PRESSED)
        return select_tab(tabs_el, tab_el,
          keyboardStates & SHIFT_KEY_PRESSED ? -1 : 1);
        break;
      case VK_LEFT: return is_in_focus(tab_el) ? select_tab(tabs_el, tab_el, -1) : FALSE;
      case VK_RIGHT: return is_in_focus(tab_el) ? select_tab(tabs_el, tab_el, 1) : FALSE;
      case VK_HOME: return is_in_focus(tab_el) ? select_tab(tabs_el, tab_el, -2) : FALSE;
      case VK_END: return is_in_focus(tab_el) ? select_tab(tabs_el, tab_el, 2) : FALSE;
      }
      return FALSE;
    }
#endif

    virtual bool on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason)
    {
      if (type == ACTIVATE_CHILD)
      {
        dom::element newtab = target_tab(target, he);
        if (!newtab.is_valid())
        {
          assert(false); // target is not a tab here.
          return TRUE;
        }
        dom::element tabs = he;
        select_tab(tabs, newtab);
        return TRUE;
      }
      return FALSE;
    }

    // select 
    bool select_tab(dom::element& tabs_el, dom::element& tab_el)
    {
      if (tab_el.get_state(STATE_CURRENT))
        // already selected, nothing to do...
        return true; // but we've handled it.

      //find currently selected element (tab and panel) and remove "selected" from them
      dom::element prev_panel_el = tabs_el.find_first(":root>:not(.strip):expanded");
      dom::element prev_tab_el = tabs_el.find_first(":root>.strip>[panel]:current");

      // find new tab and panel       
      json::string pname = tab_el.get_attribute("panel");
      dom::element panel_el = tabs_el.find_first(":root>[name=\"%s\"],:root>[id=\"%s\"]", aux::w2a(pname).c_str(), aux::w2a(pname).c_str());

      if (!panel_el.is_valid() || !tab_el.is_valid())
      {
        assert(false); // panel="somename" without matching name="somename"
        return true;
      }

      if (prev_panel_el.is_valid())
      {
        prev_panel_el.set_attribute("selected", 0); // remove selected attribute - just in case somone is using attribute selectors    
        prev_panel_el.set_state(STATE_COLLAPSED, 0); // set collapsed in case of someone use it for styling
      }
      if (prev_tab_el.is_valid())
      {
        prev_tab_el.set_attribute("selected", 0); // remove selected attribute
        prev_tab_el.set_state(0, STATE_CURRENT); // reset also state flag, :current
      }

      panel_el.set_attribute("selected", WSTR("")); // set selected attribute (empty)
      panel_el.set_state(STATE_EXPANDED, 0); // expand it

      tab_el.set_attribute("selected", WSTR("")); // set selected attribute (empty)
      tab_el.set_state(STATE_CURRENT, 0); // set also state flag, :current

      // notify all parties involved
      if (prev_tab_el.is_valid())
        prev_tab_el.post_event(ELEMENT_COLLAPSED, 0, prev_tab_el); // source here is old collapsed tab itself
      tab_el.post_event(ELEMENT_EXPANDED, 0, tab_el);  // source here is new expanded tab itself
      // NOTE #1: these event will bubble from panel elements up to the root so panel itself, tabs ctl, its parent, etc.
      // will receive these notifications. Handle them if you need to change UI dependent from current tab. 
      // NOTE #2: while handling this event in:
      //        virtual bool on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ),
      // HELEMENT target is the panel element being collapsed/expanded

      return true;
    }

    // select next/prev/first/last tab
    bool select_tab(dom::element& tabs_el, dom::element& tab_el, int direction)
    {

      // find new tab 

      dom::element new_tab_el(0);

      int rdirection = direction;

      switch (direction)
      {
      case -2: new_tab_el = tab_el.first_sibling(); rdirection = +1; break;
      case -1: new_tab_el = tab_el.prev_sibling(); break;
      case +1: new_tab_el = tab_el.next_sibling(); break;
      case +2: new_tab_el = tab_el.last_sibling(); rdirection = -1; break;
      default: assert(false); return false;
      }

      if (!new_tab_el.is_valid() || new_tab_el.get_attribute("panel").length() == 0)  //is not a tab element
        return FALSE;

      if (!new_tab_el.enabled() || !new_tab_el.visible()) // if it is either disabled or not visible - try next
        return select_tab(tabs_el, new_tab_el, rdirection);

      return select_tab(tabs_el, new_tab_el);
    }

    dom::element target_tab(HELEMENT he, HELEMENT h_tabs_container)
    {
      if (!he || he == h_tabs_container)
        return 0;

      dom::element el = he;
      sciter::string panel_name = el.get_attribute("panel");
      if (panel_name.length())
        return el; // here we are!

      return target_tab(el.parent(), h_tabs_container);
    }

    // script api

    bool select(sciter::value indexOrName) {
      dom::element tabs_el = self;
      auto all_tabs = tabs_el.find_all(":root>.strip>[panel]");
      if (indexOrName.is_int()) {
        int idx = indexOrName.get<int>();
        if (idx >= 0 && idx < int(all_tabs.size()))
          return select_tab(tabs_el, all_tabs[idx]);
      }
      else if (indexOrName.is_string()) {
        sciter::string name = indexOrName.get<sciter::string>();
        for (int n = 0; n < int(all_tabs.size()); ++n) {
          dom::element tab = all_tabs[n];
          sciter::string n_name = tab.get_attribute("panel");
          if(name == n_name)
            return select_tab(tabs_el, tab);
        }
      }
      return false;
    }

    std::vector<sciter::string> names() {
      dom::element tabs_el = self;
      auto all_tabs = tabs_el.find_all(":root>.strip>[panel]");
      std::vector<sciter::string> all_names;
      for (auto tab : all_tabs) {
        all_names.push_back(tab.get_attribute("panel"));
      }
      return all_names;
    }

    sciter::string get_current() {
      dom::element tabs_el = self;
      dom::element current_tab_el = tabs_el.find_first(":root>.strip>[panel]:current");
      if (current_tab_el)
        current_tab_el.get_attribute("panel");
      return sciter::string();
    }

    SOM_PASSPORT_BEGIN(tabs)
      SOM_FUNCS(
        SOM_FUNC(select),
        SOM_FUNC(names)
      )
      SOM_PROPS(
        SOM_RO_VIRTUAL_PROP(current,get_current),
      )
    SOM_PASSPORT_END

   
};

// instantiating and attaching it to the global list
struct tabs_factory : public behavior_factory {

  tabs_factory() : behavior_factory("tabs") {}

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
    return new tabs();
  }

};

// instantiating and attaching it to the global list
tabs_factory tabs_factory_instance;

}
