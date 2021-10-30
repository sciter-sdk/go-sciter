/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) 2003-2015, Terra Informatica Software, Inc.
 */

/*
 * DOM access methods, C++ interface, sciter::dom namespace
 */

#ifndef __sciter_dom_hpp__
#define __sciter_dom_hpp__

#pragma warning(disable:4786) //identifier was truncated...
#pragma warning(disable:4100) //unreferenced formal parameter

#include "sciter-x-dom.h"
#include <algorithm>
#include <vector>

/**sciter namespace.*/
namespace sciter
{
/**dom namespace.*/
namespace dom
{

/**callback structure.
  * Used with #sciter::dom::element::select() function.
  **/
  struct callback
  {
  /**Is called for every element that match criteria specified when calling to #sciter::dom::element::select() function.*/
    virtual bool on_element(HELEMENT he) = 0;
  };

  class element;

/**DOM node - element, comment, text */

  class node
  {
    HNODE hn;
  protected:
    void use(HNODE h) { hn = ( SciterNodeAddRef(h) == SCDOM_OK)? h: 0; }
    void unuse() { if(hn) SciterNodeRelease(hn); hn = 0; }
    void set(HNODE h) { unuse(); use(h); }
  public:
    node(): hn(0)       {}
    node(HNODE h)       { use(h); }
    node(HELEMENT h);
    node(const element& el);
    node(const node& n) { use(n.hn); }
    ~node()             { unuse(); }

    operator HNODE() const { return hn; }
    operator bool() const { return hn != 0; }

    node& operator = (HNODE h) { set(h); return *this; }
    node& operator = (HELEMENT h);

    node& operator = (const node& n) { set(n.hn); return *this; }
    node& operator = (const element& n);

    bool operator == (const node& rs ) const { return hn == rs.hn; }
    bool operator == (HNODE rs ) const { return hn == rs; }
    bool operator != (const node& rs ) const { return hn != rs.hn; }
    bool operator != (HNODE rs ) const { return hn != rs; }

    bool is_valid() const { return hn != 0; }

    HELEMENT parent( ) const;
    HNODE next_sibling() const; // sibling
    HNODE prev_sibling() const; // sibling
    HNODE first_child() const; // child
    HNODE last_child() const; // child

    unsigned int children_count() const;
    HNODE operator[](unsigned int idx) const;
    HNODE operator[](int idx) const { return this->operator[](unsigned(idx)); }

    bool is_text() const;
    bool is_comment() const;
    bool is_element() const;

    HELEMENT to_element() const;

    // get/set characters of text/comment node
    sciter::string text() const; // get text as sciter::string (utf16)
              void text(LPCWSTR text, UINT textLength) const; // set text
              void text(const sciter::string& s) { text(s.c_str(),UINT(s.length())); }

    void remove(); // remove it from the DOM and release reference
    void detach(); // detach it from the DOM and keep reference

    static node make_text_node(LPCWSTR text, UINT textLength);
    static node make_comment_node(LPCWSTR text, UINT textLength);

    void append(HNODE hn);  // as a last child node
    void prepend(HNODE hn); // as a first child node
    void insert_before(HNODE hn); // as a previous sibling
    void insert_after(HNODE hn);  // as a next sibling

    // fetch DOM node reference from SCITER_VALUE envelope
    static node from_value(const SCITER_VALUE& v) {
      HNODE hn = 0;
      SCDOM_RESULT r = SciterNodeUnwrap(&v, &hn);
      assert(r == SCDOM_OK); (void)r;
      return node(hn);
    }

    // wrap DOM node reference into sciter::value envelope
    SCITER_VALUE to_value() const {
      SCITER_VALUE v;
      SCDOM_RESULT r = SciterNodeWrap(&v, hn);
      assert(r == SCDOM_OK); (void)r;
      return v;
    }


  };


/**DOM element.*/

  class element
  {

  protected:
    HELEMENT he;

    void use(HELEMENT h) { he = ( Sciter_UseElement(h) == SCDOM_OK)? h: 0; }
    void unuse() { if(he) Sciter_UnuseElement(he); he = 0; }
    void set(HELEMENT h) { unuse(); use(h); }

  public:
  /**Construct \c undefined element .
    **/
    element(): he(0) { }

  /**Construct \c element from existing element handle.
    * \param h \b #HELEMENT
    **/
    element(HELEMENT h)       { use(h); }

  /**Copy constructor;
    * \param e \b #element
    **/
    element(const element& e) { use(e.he); }
    element(const node& e);

    operator bool() const { return he != 0; }

    operator HELEMENT() const { return he; }
    operator HNODE() const {
      HNODE hn = 0;
      SciterNodeCastFromElement(he,&hn); // always succeedes
      return hn;
    }

  /**Destructor.*/
    ~element()                { unuse(); }

  /**Assign \c element an \c #HELEMENT
    * \param h \b #HELEMENT
    * \return \b #element&
    **/
    element& operator = (HELEMENT h) { set(h); return *this; }

  /**Assign \c element another \c #element
    * \param e \b #element
    * \return \b #element&
    **/
    element& operator = (const element& e) { set(e.he); return *this; }

  /**Test equality of this and another \c #element's
    * \param rs \b const \b #element
    * \return \b bool, true if elements are equal, false otherwise
    **/
    bool operator == (const element& rs ) const { return he == rs.he; }
    bool operator == (HELEMENT rs ) const { return he == rs; }

  /**Test equality of this and another \c #element's
    * \param rs \b const \b #element
    * \return \b bool, true if elements are not equal, false otherwise
    **/
    bool operator != (const element& rs ) const { return he != rs.he; }
    bool operator != (HELEMENT rs ) const { return he != rs; }

  /**Test whether element is valid.
    * \return \b bool, true if element is valid, false otherwise
    **/
    bool is_valid() const { return he != 0; }

  /**Get number of child elements.
    * \return \b int, number of child elements
    **/
    unsigned int children_count() const
    {
      UINT count = 0;
      SciterGetChildrenCount(he, &count);
      return count;
    }

  /**Get Nth child element.
    * \param index \b unsigned \b int, number of the child element
    * \return \b #HELEMENT, child element handle
    **/
    HELEMENT child( unsigned int index ) const
    {
      HELEMENT child = 0;
      SciterGetNthChild(he, index, &child);
      return child;
    }

  /**Get parent element.
    * \return \b #HELEMENT, handle of the parent element
    **/
    HELEMENT parent( ) const
    {
      HELEMENT hparent = 0;
      SciterGetParentElement(he, &hparent);
      return hparent;
    }

  /**Get index of this element in its parent collection.
    * \return \b unsigned \b int, index of this element in its parent collection
    **/
    unsigned int index( ) const
    {
      UINT index = 0;
      SciterGetElementIndex(he, &index);
      return index;
    }

  /**Get number of the attributes.
    * \return \b unsigned \b int, number of the attributes
    **/
    unsigned int get_attribute_count( ) const
    {
      UINT n = 0;
      SciterGetAttributeCount(he, &n);
      return n;
    }

  /**Get attribute value by its index.
    * \param n \b unsigned \b int, number of the attribute
    * \return \b const \b WCHAR*, value of the n-th attribute
    **/
    sciter::string get_attribute( unsigned int n ) const
    {
      sciter::string s;
      SCDOM_RESULT r = SciterGetNthAttributeValueCB(he, n, &_LPCWSTR2STRING, &s);
      assert(r == SCDOM_OK); (void)r;
      return s;
    }

  /**Get attribute name by its index.
    * \param n \b unsigned \b int, number of the attribute
    * \return \b const \b char*, name of the n-th attribute
    **/
    sciter::astring get_attribute_name( unsigned int n ) const
    {
      sciter::astring s;
      SCDOM_RESULT r = SciterGetNthAttributeNameCB(he, n, &_LPCSTR2ASTRING, &s);
      assert(r == SCDOM_OK); (void)r;
      return s;
    }

  /**Get attribute value by name.
    * \param name \b const \b char*, name of the attribute
    * \return \b sciter::string, value of the n-th attribute
    **/
    sciter::string get_attribute( const char* name, const WCHAR* def_value = 0 ) const
    {
      sciter::string s;
      SCDOM_RESULT r = SciterGetAttributeByNameCB(he, name, &_LPCWSTR2STRING, &s);
      if(r == SCDOM_OK_NOT_HANDLED && def_value)
        return sciter::string(def_value);
      return s;
    }

  /**Add or replace attribute.
    * \param name \b const \b char*, name of the attribute
    * \param value \b const \b WCHAR*, name of the attribute
    **/
  void set_attribute( const char* name, const WCHAR* value )
    {
      SciterSetAttributeByName(he, name, value);
    }

  /**Get attribute integer value by name.
    * \param name \b const \b char*, name of the attribute
    * \return \b int , value of the attribute
    **/
    int get_attribute_int( const char* name, int def_val = 0 ) const
    {
      sciter::string txt = get_attribute(name);
      if(txt.length() == 0) return def_val;
      aux::wchars wc = aux::chars_of(txt);
      return aux::to_int(wc);
    }


  /**Remove attribute.
    * \param name \b const \b char*, name of the attribute
    **/
  void remove_attribute( const char* name )
    {
      SciterSetAttributeByName(he, name, 0);
    }


  /**Get style attribute of the element by its name.
    * \param name \b const \b char*, name of the style attribute, e.g. "background-color"
    * \return \b const \b WCHAR*, value of the style attribute
    **/
  sciter::string get_style_attribute( const char* name ) const
    {
      sciter::string s;
      SciterGetStyleAttributeCB(he, name,&_LPCWSTR2STRING, &s);
      return s;
    }

  /**Set style attribute.
    * \param name \b const \b char*, name of the style attribute
    * \param value \b const \b WCHAR*, value of the style attribute
    *
    * \par Example:
    * \code e.set_style_attribute("background-color", L"red"); \endcode
    **/
  void set_style_attribute( const char* name, const WCHAR* value ) const
    {
      SciterSetStyleAttribute(he, name, value);
    }

  /**Get root DOM element of the Sciter document.
    * \param hSciterWnd \b HWINDOW, Sciter window
    * \return \b #HELEMENT, root element
    * \see also \b #root
    **/
    static HELEMENT root_element(HWINDOW hSciterWnd)
    {
      HELEMENT h = 0;
      SciterGetRootElement(hSciterWnd,&h);
      return h;
    }

  /**Get focus DOM element of the Sciter document.
    * \param hSciterWnd \b HWINDOW, Sciter window
    * \return \b #HELEMENT, focus element
    *
    * COMMENT: to set focus use: set_state(STATE_FOCUS)
    *
    **/
    static HELEMENT focus_element(HWINDOW hSciterWnd)
    {
      HELEMENT h = 0;
      SciterGetFocusElement(hSciterWnd,&h);
      return h;
    }


  /**Find DOM element of the Sciter document by coordinates.
    * \param hSciterWnd \b HWINDOW, Sciter window
    * \param clientPt \b POINT,  coordinates.
    * \return \b #HELEMENT, found element handle or zero
    **/
    static HELEMENT find_element(HWINDOW hSciterWnd, POINT clientPt)
    {
      HELEMENT h = 0;
      SciterFindElement(hSciterWnd, clientPt, &h);
      return h;
    }


  /**Set mouse capture.
    * After call to this function all mouse events will be targeted to this element.
    * To remove mouse capture call #sciter::dom::element::release_capture().
    **/
    void set_capture() { SciterSetCapture(he); }

  /**Release mouse capture.
    * Mouse capture can be set with #element:set_capture()
    **/
    void release_capture() { SciterReleaseCapture(he); }

    inline static SBOOL SC_CALLBACK callback_func( HELEMENT he, LPVOID param )
    {
      callback *pcall = (callback *)param;
      return (SBOOL)pcall->on_element(he); // SBOOL(true) - stop enumeration
    }

    inline void select_elements( callback *pcall,
                        const char* selectors // CSS selectors, comma separated list
                      ) const
    {
      SciterSelectElements( he, selectors, callback_func, pcall);
    }


    /**Get element by id.
    * \param id \b char*, value of the "id" attribute.
    * \return \b #HELEMENT, handle of the first element with the "id" attribute equal to given.
    **/
    HELEMENT get_element_by_id(const char* id) const
    {
      if(!id) return 0;
      return find_first( "[id='%s']", id );
    }

    HELEMENT get_element_by_id(const WCHAR* id) const
    {
      if(!id) return 0;
      return find_first( "[id='%S']", id );
    }


  /**Apply changes and refresh element area in its window.
    * \param[in] render_now \b bool, if true element will be redrawn immediately.
    **/
    void update( bool render_now = false ) const
    {
      SciterUpdateElement(he, (SBOOL)render_now);
    }

    void refresh( RECT rc ) const
    {
      SciterRefreshElementArea(he, rc);
    }

    void refresh( ) const
    {
      RECT rc = get_location(SELF_RELATIVE | CONTENT_BOX);
      refresh( rc );
    }


  /**Get next sibling element.
    * \return \b #HELEMENT, handle of the next sibling element if it exists or 0 otherwise
    **/
    HELEMENT next_sibling() const
    {
      unsigned int idx = index() + 1;
      element pel = parent();
      if(!pel.is_valid())
        return 0;
      if( idx >= pel.children_count() )
        return 0;
      return pel.child(idx);
   }

  /**Get previous sibling element.
    * \return \b #HELEMENT, handle of previous sibling element if it exists or 0 otherwise
    **/
    HELEMENT prev_sibling() const
    {
      int idx = static_cast<int>(index()) - 1;
      element pel = parent();
      if(!pel.is_valid())
        return 0;
      if( idx < 0 )
        return 0;
      return pel.child(static_cast<unsigned>(idx));
    }

  /**Get first sibling element.
    * \return \b #HELEMENT, handle of the first sibling element if it exists or 0 otherwise
    **/
    HELEMENT first_sibling() const
    {
      element pel = parent();
      if(!pel.is_valid())
        return 0;
      return pel.child(0);
    }

  /**Get last sibling element.
    * \return \b #HELEMENT, handle of last sibling element if it exists or 0 otherwise
    **/
    HELEMENT last_sibling() const
    {
      element pel = parent();
      if(!pel.is_valid())
        return 0;
      return pel.child(pel.children_count() - 1);
    }


  /**Get root of the element
    * \return \b #HELEMENT, handle of document root element (html)
    **/
    HELEMENT root() const
    {
      element pel = parent();
      if(pel.is_valid())
        return pel.root();
      return he;
    }

  /**Get bounding rectangle of the element.
    * \param root_relative \b bool, if true function returns location of the
    * element relative to Sciter window, otherwise the location is given
    * relative to first scrollable container.
    * \return \b RECT, bounding rectangle of the element.
    **/
    RECT get_location(unsigned int area = ROOT_RELATIVE | CONTENT_BOX) const
    {
      RECT rc = {0,0,0,0};
      SciterGetElementLocation(he,&rc, area);
      return rc;
    }

    /** Test if point is inside shape rectangle of the element.
      client_pt - client rect relative point
      **/
    bool is_inside( POINT client_pt ) const
    {
      RECT rc = get_location(ROOT_RELATIVE | BORDER_BOX);
      return client_pt.x >= rc.left
          && client_pt.x < rc.right
          && client_pt.y >= rc.top
          && client_pt.y < rc.bottom;
    }

    /**Scroll this element to view.
      **/
    void scroll_to_view(bool toTopOfView = false, bool smooth = false)
    {
      UINT flags = 0;
      if(toTopOfView)  flags |= SCROLL_TO_TOP;
      if(smooth) flags |= SCROLL_SMOOTH;
      SciterScrollToView(he, flags );
    }

    void get_scroll_info(POINT& scroll_pos, RECT& view_rect, SIZE& content_size)
    {
      SCDOM_RESULT r = SciterGetScrollInfo(he, &scroll_pos, &view_rect, &content_size);
      assert(r == SCDOM_OK); (void)r;
    }
    void set_scroll_pos(POINT scroll_pos)
    {
      SCDOM_RESULT r = SciterSetScrollPos(he, scroll_pos, true);
      assert(r == SCDOM_OK); (void)r;
    }

    /** get min-intrinsic and max-intrinsic widths of the element. */
    void get_intrinsic_widths(int& min_width,int& max_width)
    {
      SCDOM_RESULT r = SciterGetElementIntrinsicWidths(he, &min_width, &max_width);
      assert(r == SCDOM_OK); (void)r;
    }
    /** get min-intrinsic height of the element calculated for forWidth. */
    void get_intrinsic_height(int for_width, int& min_height)
    {
      SCDOM_RESULT r = SciterGetElementIntrinsicHeight(he, for_width, &min_height);
      assert(r == SCDOM_OK); (void)r;
    }

    /**Get element's tag name.
    * \return \b sciter::astring, tag name of the element
    *
      * \par Example:
      * For &lt;div&gt; tag function will return "div".
    **/
    sciter::astring get_element_type() const
    {
      sciter::astring s;
      SciterGetElementTypeCB(he, &_LPCSTR2ASTRING, &s);
      return s;
    }
    // alias
    sciter::astring get_tag() const { return get_element_type(); }

  /**Get HWINDOW of containing window.
    * \param root_window \b bool, handle of which window to get:
    * - true - Sciter window
    * - false - nearest windowed parent element.
    * \return \b HWINDOW
    **/
    HWINDOW get_element_hwnd(bool root_window) const
    {
      HWINDOW hwnd = 0;
      SciterGetElementHwnd(he,&hwnd, (SBOOL)root_window);
      return hwnd;
    }

    void attach_hwnd(HWINDOW child)
    {
      SCDOM_RESULT r = SciterAttachHwndToElement(he,child);
      assert( r == SCDOM_OK ); (void)r;
    }

  /**Get element UID - identifier suitable for storage.
    * \return \b UID
    **/
    UINT get_element_uid() const
    {
      UINT uid = 0;
      SciterGetElementUID(he,&uid);
      return uid;
    }

  /**Get element handle by its UID.
    * \param hSciterWnd \b HWINDOW, Sciter window
    * \param uid \b UINT, uid of the element
    * \return \b #HELEMENT, handle of element with the given uid or 0 if not found
    **/
    static HELEMENT element_by_uid(HWINDOW hSciterWnd, UINT uid)
    {
      HELEMENT h = 0;
      SciterGetElementByUID(hSciterWnd, uid,&h);
      return h;
    }

  /**Combine given URL with URL of the document element belongs to.
    * \param[in, out] inOutURL \b LPWSTR, at input this buffer contains
    * zero-terminated URL to be combined, after function call it contains
    * zero-terminated combined URL
    * \param bufferSize \b UINT, size of the buffer pointed by \c inOutURL
      **/
    void combine_url(LPWSTR inOutURL, UINT bufferSize) const
    {
      SciterCombineURL(he,inOutURL,bufferSize);
    }

#ifdef CPP11
    sciter::string combine_url(const sciter::string& relative_url) const
    {
      WCHAR buffer[4096] = {0};
      //wcsncpy_s(buffer,relative_url,min(4096,relative_url.length()));
      std::size_t length = relative_url.length() < 4095u ? relative_url.length(): 4095u ;
      std::copy_n(relative_url.cbegin(), length, buffer);
      buffer[length] = 0;
      SciterCombineURL(he,buffer,4096);
      return sciter::string(buffer);
    }
#endif

  /**Set inner or outer html of the element.
    * \param html \b const \b unsigned \b char*, UTF-8 encoded string containing html text
    * \param html_length \b size_t, length in bytes of \c html
    * \param where \b int, possible values are:
    * - SIH_REPLACE_CONTENT - replace content of the element
    * - SIH_INSERT_AT_START - insert html before first child of the element
    * - SIH_APPEND_AFTER_LAST - insert html after last child of the element
    **/
    void set_html( const unsigned char* html, size_t html_length, int where = SIH_REPLACE_CONTENT)
    {
      if(html == 0 || html_length == 0)
        clear();
      else
      {
        SCDOM_RESULT r = SciterSetElementHtml(he, html, UINT(html_length), where);
        assert(r == SCDOM_OK); (void)r;
      }
    }

    // html as utf8 bytes sequence
    sciter::astring
        get_html( bool outer = true) const
    {
      sciter::astring s;
      SCDOM_RESULT r = SciterGetElementHtmlCB(he, SBOOL(outer), &_LPCBYTE2ASTRING,&s);
      assert(r == SCDOM_OK); (void)r;
      return s;
    }

    // get text as sciter::string (utf16)
    sciter::string text() const
    {
      sciter::string s;
      SCDOM_RESULT r = SciterGetElementTextCB(he, &_LPCWSTR2STRING, &s);
      assert(r == SCDOM_OK); (void)r;
      return s;
    }

    void  set_text(const WCHAR* utf16, size_t utf16_length)
    {
      SCDOM_RESULT r = SciterSetElementText(he, utf16, UINT(utf16_length));
      assert(r == SCDOM_OK); (void)r;
    }

    void  set_text(const WCHAR* t)
    {
      assert(t);
      if( t ) set_text( t, str_length(t) );
    }

    void clear() // clears content of the element
    {
      SCDOM_RESULT r = SciterSetElementText(he, 0, 0);
      assert(r == SCDOM_OK); (void)r;
    }

    HELEMENT find_first( const char* selector, ... ) const
    {
      char buffer[2049]; buffer[0]=0;
      va_list args;
      va_start ( args, selector );
      vsnprintf( buffer, sizeof(buffer), selector, args );
      va_end ( args );
      find_first_callback find_first;
      select_elements( &find_first, buffer); // find first element satisfying given CSS selector
      //assert(find_first.hfound);
      return find_first.hfound;
    }

    void find_all( callback* cb, const char* selector, ... ) const
    {
      char buffer[2049]; buffer[0]=0;
      va_list args;
      va_start ( args, selector );
      vsnprintf( buffer, sizeof(buffer), selector, args );
      va_end ( args );
      select_elements( cb, buffer); // find all elements satisfying given CSS selector
      //assert(find_first.hfound);
    }

#ifdef CPP11
    std::vector<sciter::dom::element> 
      find_all(const char* selector, ...) const
    {
      struct each_callback : public sciter::dom::callback
      {
        std::vector<sciter::dom::element> elements;
        virtual bool on_element(HELEMENT he) {
          elements.push_back(sciter::dom::element(he));
          return false; // no stop
        }
      };
      each_callback cb;
      this->find_all(&cb, selector);
      return cb.elements;
    }
#endif

    // will find first parent satisfying given css selector(s)
    HELEMENT find_nearest_parent(const char* selector, ...) const
    {
      char buffer[2049]; buffer[0]=0;
      va_list args;
      va_start ( args, selector );
      vsnprintf( buffer, sizeof(buffer), selector, args );
      va_end ( args );

      HELEMENT heFound = 0;
      SCDOM_RESULT r = SciterSelectParent(he, buffer, 0, &heFound);
      assert(r == SCDOM_OK); (void)r;
      return heFound;
    }

    // test this element against CSS selector(s)
    bool test(const char* selector, ...) const
    {
      char buffer[2049]; buffer[0]=0;
      va_list args;
      va_start ( args, selector );
      vsnprintf( buffer, sizeof(buffer), selector, args );
      va_end ( args );
      HELEMENT heFound = 0;
      SCDOM_RESULT r = SciterSelectParent(he, buffer, 1, &heFound);
      assert(r == SCDOM_OK); (void)r;
      return heFound != 0;
    }


  /**Get UI state bits of the element as set of ELEMENT_STATE_BITS
    **/
    unsigned int get_state() const
    {
      UINT state = 0;
      SCDOM_RESULT r = SciterGetElementState(he,&state);
      assert(r == SCDOM_OK); (void)r;
      return state; /*ELEMENT_STATE_BITS*/
    }

  /**Checks if particular UI state bits are set in the element.
    **/
    bool get_state(/*ELEMENT_STATE_BITS*/ unsigned int bits) const
    {
      UINT state = 0;
      SCDOM_RESULT r = SciterGetElementState(he,&state);
      assert(r == SCDOM_OK); (void)r;
      return (state & bits) != 0;
    }


  /**Set UI state of the element with optional view update.
    **/
    void set_state(
      /*ELEMENT_STATE_BITS*/ unsigned int bitsToSet,
      /*ELEMENT_STATE_BITS*/ unsigned int bitsToClear = 0, bool update = true )
    {
      SCDOM_RESULT r = SciterSetElementState(he,bitsToSet,bitsToClear, SBOOL(update));
      assert(r == SCDOM_OK); (void)r;
    }

    /** "deeply enabled" **/
    bool enabled()
    {
      SBOOL b = false;
      SCDOM_RESULT r = SciterIsElementEnabled(he,&b);
      assert(r == SCDOM_OK); (void)r;
      return b != 0;
    }

    /** "deeply visible" **/
    bool visible()
    {
      SBOOL b = false;
      SCDOM_RESULT r = SciterIsElementVisible(he,&b);
      assert(r == SCDOM_OK); (void)r;
      return b != 0;
    }

    void start_timer(unsigned int ms, void* timer_id = 0)
    {
      SCDOM_RESULT r = SciterSetTimer(he,ms,UINT_PTR(timer_id));
      assert(r == SCDOM_OK); (void)r;
    }
    void stop_timer(void* timer_id = 0)
    {
      if(he)
      {
        SCDOM_RESULT r = SciterSetTimer(he,0,UINT_PTR(timer_id));
        assert(r == SCDOM_OK); (void)r;
      }
    }


  /** create brand new element with text (optional).
      Example:
          element div = element::create("div");
      - will create DIV element,
          element opt = element::create("option",L"Europe");
      - will create OPTION element with text "Europe" in it.
    **/
    static element create(const char* tagname, const WCHAR* text = 0)
    {
        element e(0);
        SCDOM_RESULT r = SciterCreateElement( tagname, text, &e.he ); // don't need 'use' here, as it is already "addrefed"
        assert(r == SCDOM_OK); (void)r;
        return e;
    }

  /** create brand new copy of this element. Element will be created disconected.
      You need to call insert to inject it in some container.
      Example:
          element select = ...;
          element option1 = ...;
          element option2 = option1.clone();
          select.insert(option2, option1.index() + 1);
      - will create copy of option1 element (option2) and insert it after option1,
    **/
    element clone()
    {
        element e(0);
        SCDOM_RESULT r = SciterCloneElement( he, &e.he ); // don't need 'use' here, as it is already "addrefed"
        assert(r == SCDOM_OK); (void)r;
        return e;
    }


  /** Insert element e at \i index position of this element.
    **/
    void insert( const element& e, unsigned int index )
    {
        SCDOM_RESULT r = SciterInsertElement( e.he, this->he, index );
        assert(r == SCDOM_OK); (void)r;
    }

  /** Append element e as last child of this element.
    **/
    void append( const element& e ) { insert(e,0x7FFFFFFF); }


    /** detach - remove this element from its parent
    **/
    void detach()
    {
      SCDOM_RESULT r = SciterDetachElement( he );
      assert(r == SCDOM_OK); (void)r;
    }

    /** destroy - remove this element from its parent and destroy all behaviors
     **/
    void destroy()
    {
		  HELEMENT t = he; he = 0;
      SCDOM_RESULT r = SciterDeleteElement( t );
      assert(r == SCDOM_OK); (void)r;
    }

    /** swap two elements in the DOM
    **/
    void swap(HELEMENT with)
    {
        SCDOM_RESULT r = SciterSwapElements(he, with);
        assert(r == SCDOM_OK); (void)r;
    }

    /** traverse event - send it by sinking/bubbling on the
      * parent/child chain of this element
      **/
    bool send_event(unsigned int event_code, uintptr_t reason = 0, HELEMENT heSource = 0)
    {
      SBOOL handled = false;
      SCDOM_RESULT r = SciterSendEvent(he, event_code, heSource? heSource: he, reason, &handled);
      assert(r == SCDOM_OK); (void)r;
      return handled != 0;
    }

    /** post event - post it in the queue for later sinking/bubbling on the
      * parent/child chain of this element.
      * method returns immediately
      **/
    void post_event(unsigned int event_code, uintptr_t reason = 0, HELEMENT heSource = 0)
    {
      SCDOM_RESULT r = SciterPostEvent(he, event_code, heSource? heSource: he, reason);
      assert(r == SCDOM_OK); (void)r;
    }

    bool fire_event(const BEHAVIOR_EVENT_PARAMS& evt, bool post = true)
    {
      SBOOL handled = false;
      SCDOM_RESULT r = SciterFireEvent(&evt, post, &handled);
      assert(r == SCDOM_OK); (void)r;
      return handled != 0;
    }


    /** call method, invokes method in all event handlers attached to the element
      **/
    bool call_behavior_method(METHOD_PARAMS* p)
    {
      if(!is_valid())
        return false;
      return SciterCallBehaviorMethod(he,p) == SCDOM_OK;
    }

    void load_html(const WCHAR* url, HELEMENT initiator = 0)
    {
      load_data(url,RT_DATA_HTML, initiator);
    }

    void load_data(const WCHAR* url, UINT dataType, HELEMENT initiator = 0)
    {
      SCDOM_RESULT r = SciterRequestElementData(he,url, dataType, initiator);
      assert(r == SCDOM_OK); (void)r;
    }


    struct comparator
    {
      virtual int compare(const sciter::dom::element& e1, const sciter::dom::element& e2) = 0;

      static INT SC_CALLBACK scmp( HELEMENT he1, HELEMENT he2, LPVOID param )
      {
        sciter::dom::element::comparator* self =
          static_cast<sciter::dom::element::comparator*>(param);

        sciter::dom::element e1 = he1;
        sciter::dom::element e2 = he2;

        return self->compare( e1,e2 );
      }
    };

    /** reorders children of the element using sorting order defined by cmp
      **/
    void sort( comparator& cmp, int start = 0, int end = -1 )
    {
      if (end == -1)
        end = children_count();

      SCDOM_RESULT r = SciterSortElements(he, start, end, &comparator::scmp, &cmp);
      assert(r == SCDOM_OK); (void)r;
    }

    // "manually" attach event_handler proc to the DOM element
    void attach_event_handler(event_handler* p_event_handler )
    {
      SciterAttachEventHandler(he, &event_handler_raw::element_proc, static_cast<event_handler_raw*>(p_event_handler));
    }

    void detach_event_handler(event_handler* p_event_handler )
    {
      SciterDetachEventHandler(he, &event_handler_raw::element_proc, static_cast<event_handler_raw*>(p_event_handler));
    }

    // call scripting method attached to the element (directly or through scripting behavior)
    // Example, script:
    //   var elem = ...
    //   elem.foo = function() {...}
    // Native code:
    //   dom::element elem = ...
    //   elem.call_method("foo");
    SCITER_VALUE  call_method(LPCSTR name, UINT argc, SCITER_VALUE* argv )
    {
      SCITER_VALUE rv = SCITER_VALUE();
      SCDOM_RESULT r = SciterCallScriptingMethod(he, name, argv,argc, &rv);
#if !defined(SCITER_SUPPRESS_SCRIPT_ERROR_THROW)
      if( (r != SCDOM_OK) && rv.is_error_string()) {
        aux::w2a reason(rv.get(WSTR("")));
        throw sciter::script_error(reason.c_str());
      }
#endif
      assert(r == SCDOM_OK); (void)r;
      return rv;
    }

    // flattened wrappers of the above. note SCITER_VALUE is a json::value
    SCITER_VALUE  call_method(LPCSTR name )
    {
      return call_method(name,0,0);
    }
    // flattened wrappers of the above. note SCITER_VALUE is a json::value
    SCITER_VALUE  call_method(LPCSTR name, SCITER_VALUE arg0 )
    {
      return call_method(name,1,&arg0);
    }
    SCITER_VALUE  call_method(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1 )
    {
      SCITER_VALUE argv[2]; argv[0] = arg0; argv[1] = arg1;
      return call_method(name,2,argv);
    }
    SCITER_VALUE  call_method(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2 )
    {
      SCITER_VALUE argv[3]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2;
      return call_method(name,3,argv);
    }
    SCITER_VALUE  call_method(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2, SCITER_VALUE arg3 )
    {
      SCITER_VALUE argv[4]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2; argv[3] = arg3;
      return call_method(name,4,argv);
    }

    // call scripting function defined on global level
    // Example, script:
    //   function foo() {...}
    // Native code:
    //   dom::element root = ... get root element of main document or some frame inside it
    //   root.call_function("foo"); // call the function

    SCITER_VALUE  call_function(LPCSTR name, UINT argc, SCITER_VALUE* argv )
    {
      SCITER_VALUE rv;
      SCDOM_RESULT r = SciterCallScriptingFunction(he, name, argv,argc, &rv);
#if !defined(SCITER_SUPPRESS_SCRIPT_ERROR_THROW)
      if( (r != SCDOM_OK) && rv.is_error_string()) {
        aux::w2a reason(rv.get(WSTR("")));
        throw sciter::script_error(reason.c_str());
      }
#endif
      assert(r == SCDOM_OK); (void)r;
      return rv;
    }

    // flattened wrappers of the above. note SCITER_VALUE is a json::value
    SCITER_VALUE  call_function(LPCSTR name )
    {
      return call_function(name,0,0);
    }
    // flattened wrappers of the above. note SCITER_VALUE is a json::value
    SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0 )
    {
      return call_function(name,1,&arg0);
    }
    SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1 )
    {
      SCITER_VALUE argv[2]; argv[0] = arg0; argv[1] = arg1;
      return call_function(name,2,argv);
    }
    SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2 )
    {
      SCITER_VALUE argv[3]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2;
      return call_function(name,3,argv);
    }
    SCITER_VALUE  call_function(LPCSTR name, SCITER_VALUE arg0, SCITER_VALUE arg1, SCITER_VALUE arg2, SCITER_VALUE arg3 )
    {
      SCITER_VALUE argv[4]; argv[0] = arg0; argv[1] = arg1; argv[2] = arg2; argv[3] = arg3;
      return call_function(name,4,argv);
    }

    // evaluate script in element context:
    // 'this' in script will be the element
    // and in namespace of element's document.
    SCITER_VALUE eval(const WCHAR* script, size_t script_length)
    {
      SCITER_VALUE rv;
      SCDOM_RESULT r = SciterEvalElementScript( he, script, UINT(script_length), &rv );
      assert(r == SCDOM_OK); (void)r;
      return rv;
    }
    SCITER_VALUE eval(aux::wchars script)
    {
      return eval(script.start,script.length);
    }

    CTL_TYPE get_ctl_type() const
    {
      UINT t = 0;
      SCDOM_RESULT r = ::SciterControlGetType(he,&t);
      assert(r == SCDOM_OK); (void)r;
      return CTL_TYPE(t);
    }

    SCITER_VALUE get_value()
    {
      SCITER_VALUE rv;
      SCDOM_RESULT r = SciterGetValue(he, &rv);
      assert(r == SCDOM_OK); (void)r;
      return rv;
    }

    void set_value(const SCITER_VALUE& v)
    {
      SCDOM_RESULT r = SciterSetValue(he, &v);
      assert(r == SCDOM_OK); (void)r;
    }

    // fetch DOM element reference from SCITER_VALUE envelope
    static element from_value(const SCITER_VALUE& v) {
      //element el = (HELEMENT)v.get_object_data();
      HELEMENT hel = 0;
      SCDOM_RESULT r = SciterElementUnwrap(&v, &hel);
      assert(r == SCDOM_OK); (void)r;
      return element(hel);
    }

    // wrap DOM element reference into sciter::value envelope
    SCITER_VALUE to_value() const {
      SCITER_VALUE v;
      SCDOM_RESULT r = SciterElementWrap(&v, he);
      assert(r == SCDOM_OK); (void)r;
      return v;
    }

    SCITER_VALUE as_value() { return to_value(); }
        
    struct find_first_callback: callback
    {
      HELEMENT hfound;
      find_first_callback():hfound(0) {}
      inline bool on_element(HELEMENT hfe) { hfound = hfe; return true; /*stop enumeration*/ }
    };

    static HELEMENT get_highlighted(HWINDOW hSciterWnd)
    {
      HELEMENT h = 0;
      SCDOM_RESULT r = SciterGetHighlightedElement(hSciterWnd,&h);
      assert(r == SCDOM_OK); (void)r;
      return h;
    }

    void highlight()
    {
      HWINDOW hwnd = get_element_hwnd(true);
      set_highlighted(hwnd);
    }

    void set_highlighted(HWINDOW hSciterWnd)
    {
      SCDOM_RESULT r = SciterSetHighlightedElement(hSciterWnd,he);
      assert(r == SCDOM_OK); (void)r;
    }
    static void remove_highlightion(HWINDOW hSciterWnd)
    {
      SCDOM_RESULT r = SciterSetHighlightedElement(hSciterWnd,0);
      assert(r == SCDOM_OK); (void)r;
    }
  };

  inline element::element(const node& n)
  {
    use(n.to_element());
  }

  // node methods implementations

  inline node::node(const element& e):hn(0)
  {
    use(e);
    assert(hn);
  }

  inline node::node(HELEMENT h):hn(0)
  {
    use(element(h));
    assert(hn);
  }

  inline node& node::operator = (HELEMENT h)
  {
    set(element(h));
    return *this;
  }

  inline node& node::operator = (const element& n)
  {
    set(n);
    return *this;
  }

  inline unsigned int node::children_count() const
  {
    UINT n = 0;
    SCDOM_RESULT r = SciterNodeChildrenCount(hn,&n);
    assert(r == SCDOM_OK); (void)r;
    return n;
  }

  inline HNODE node::operator[](unsigned int idx) const
  {
    HNODE hrn = 0;
    SCDOM_RESULT r = SciterNodeNthChild(hn,idx,&hrn);
    assert(r == SCDOM_OK); (void)r;
    return hrn;
  }

  inline bool node::is_text() const
  {
    UINT nodeType = UINT(-1);
    SCDOM_RESULT r = SciterNodeType(hn, &nodeType);
    assert(r == SCDOM_OK); (void)r;
    return nodeType == NT_TEXT;
  }

  inline bool node::is_comment() const
  {
    UINT nodeType = UINT(-1);
    SCDOM_RESULT r = SciterNodeType(hn, &nodeType);
    assert(r == SCDOM_OK); (void)r;
    return nodeType == NT_COMMENT;
  }

  inline bool node::is_element() const
  {
    UINT nodeType = UINT(-1);
    SCDOM_RESULT r = SciterNodeType(hn, &nodeType);
    assert(r == SCDOM_OK); (void)r;
    return nodeType == NT_ELEMENT;
  }

  inline HELEMENT node::to_element() const
  {
    HELEMENT he = 0;
    SCDOM_RESULT r = SciterNodeCastToElement(hn, &he);
    assert(r == SCDOM_OK); (void)r;
    return he;
  }

  inline HELEMENT node::parent( ) const
  {
    HELEMENT heParent = 0;
    SCDOM_RESULT r = SciterNodeParent(hn, &heParent);
    if(r == SCDOM_OK_NOT_HANDLED)
      return 0;
    assert(r == SCDOM_OK); (void)r;
    return heParent;
  }

  inline HNODE node::next_sibling() const
  {
    HNODE hrn = 0;
    SCDOM_RESULT r = SciterNodeNextSibling(hn,&hrn);
    if(r == SCDOM_OK_NOT_HANDLED)
      return 0;
    assert(r == SCDOM_OK); (void)r;
    return hrn;
  }
  inline HNODE node::prev_sibling() const
  {
    HNODE hrn = 0;
    SCDOM_RESULT r = SciterNodePrevSibling(hn,&hrn);
    if(r == SCDOM_OK_NOT_HANDLED)
      return 0;
    assert(r == SCDOM_OK); (void)r;
    return hrn;
  }

  inline HNODE node::first_child() const
  {
    HNODE hrn = 0;
    SCDOM_RESULT r = SciterNodeFirstChild(hn,&hrn);
    assert(r == SCDOM_OK); (void)r;
    return hrn;
  }

  inline HNODE node::last_child() const
  {
    HNODE hrn = 0;
    SCDOM_RESULT r = SciterNodeLastChild(hn,&hrn);
    assert(r == SCDOM_OK); (void)r;
    return hrn;
  }

  inline sciter::string node::text() const // get text as sciter::string (utf16)
  {
    sciter::string s;
    SCDOM_RESULT r = SciterNodeGetText(hn, &_LPCWSTR2STRING, &s);
    assert(r == SCDOM_OK); (void)r;
    return s;
  }

  inline void node::text(LPCWSTR text, UINT textLength) const // get text as sciter::string (utf16)
  {
    SCDOM_RESULT r = SciterNodeSetText(hn, text,textLength);
    assert(r == SCDOM_OK); (void)r;
  }

  inline node node::make_text_node(LPCWSTR text, UINT textLength)
  {
    node rn;
    SCDOM_RESULT r = SciterCreateTextNode(text,textLength,&rn.hn);
    assert(r == SCDOM_OK); (void)r;
    return rn;
  }

  inline node node::make_comment_node(LPCWSTR text, UINT textLength)
  {
    node rn;
    SCDOM_RESULT r = SciterCreateCommentNode(text,textLength,&rn.hn);
    assert(r == SCDOM_OK); (void)r;
    return rn;
  }

  inline void node::remove()
  {
    SCDOM_RESULT r = SciterNodeRemove(hn,SBOOL(true));
    assert(r == SCDOM_OK); (void)r;
    SciterNodeRelease(hn);
    hn = 0;
  }

  inline void node::detach()
  {
    SCDOM_RESULT r = SciterNodeRemove(hn,SBOOL(false));
    assert(r == SCDOM_OK); (void)r;
  }

  inline void node::append(HNODE thatnode)
  {
    SCDOM_RESULT r = SciterNodeInsert(hn,NIT_APPEND,thatnode);
    assert(r == SCDOM_OK); (void)r;
  }

  inline void node::prepend(HNODE thatnode)
  {
    SCDOM_RESULT r = SciterNodeInsert(hn,NIT_PREPEND,thatnode);
    assert(r == SCDOM_OK); (void)r;
  }

  inline void node::insert_before(HNODE thatnode)
  {
    SCDOM_RESULT r = SciterNodeInsert(hn,NIT_BEFORE,thatnode);
    assert(r == SCDOM_OK); (void)r;
  }

  inline void node::insert_after(HNODE thatnode)
  {
    SCDOM_RESULT r = SciterNodeInsert(hn,NIT_AFTER,thatnode);
    assert(r == SCDOM_OK); (void)r;
  }




} // dom namespace

} // sciter namespace

#endif
