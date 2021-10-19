#include "stdafx.h"
#include "sciter-x.h"
#include "sciter-x-behavior.h"
#include "sciter-x-graphics.hpp"

namespace sciter
{
/*
BEHAVIOR: gdi-drawing
  - draws content layer using GDI primitives.

SAMPLE:
   See: samples/behaviors/gdi-drawing.htm
*/

// dib (bitmap) combined with DC

class dib32
  {
    const unsigned _width;
    const unsigned _height;
    void*      _bits;
    mutable HBITMAP    _old_bitmap;
    mutable HDC        _dc;
    HBITMAP    _bitmap;

    BITMAPINFO _bitmap_info;

  public:

    dib32(unsigned w, unsigned h) :
      _width(w),
      _height(h),
      _bits(0),
      _old_bitmap(0),
      _dc(0) {

      memset(&_bitmap_info,0,sizeof(_bitmap_info));
      _bitmap_info.bmiHeader.biSize = sizeof(_bitmap_info.bmiHeader);
      _bitmap_info.bmiHeader.biWidth = _width;
      _bitmap_info.bmiHeader.biHeight = 0 - int(_height);
      _bitmap_info.bmiHeader.biPlanes = 1;
      _bitmap_info.bmiHeader.biBitCount = 32;
      _bitmap_info.bmiHeader.biCompression = BI_RGB;

      _bitmap = ::CreateDIBSection(
            NULL, // device context
            &_bitmap_info,
            DIB_RGB_COLORS,
            &_bits,
            0, // file mapping object
            0); // file offset

      if (0 == _bits) {
        return;
        //throw std::bad_alloc();
      }

      memset(_bits,0, _width * _height * 4);

    }

    ~dib32() {
      if( _dc )
      {
        ::SelectObject(_dc,_old_bitmap);
        ::DeleteDC(_dc);
      }
      if( _bitmap )
        ::DeleteObject(_bitmap);
    }

    void set_white()
    {
      memset(_bits,0xff, _width * _height * 4);
    }

    unsigned  width() const { return _width; }
    unsigned  height() const { return _height; }
    void* bits() const { return _bits; }
    BYTE* bytes() const { return (BYTE*)_bits; }
    HDC   DC() const 
    { 
      if(!_dc) {
        _dc = ::CreateCompatibleDC(NULL);
        if (_dc)
          _old_bitmap = (HBITMAP)::SelectObject(_dc,_bitmap);
      }
      return _dc; 
    }

    HBITMAP   detach() { auto r = _bitmap; _bitmap = 0; return r; }

    const BITMAPINFO& get_info() const { return _bitmap_info; }
  };


struct gdi_drawing: public event_handler
{
    // ctor
    gdi_drawing() {}
    virtual ~gdi_drawing() {}

    virtual bool subscription( HELEMENT he, UINT& event_groups )
    {
      event_groups = HANDLE_DRAW;  // it does drawing
      return true;
    }

    virtual void attached  (HELEMENT he ) { }
    virtual void detached  (HELEMENT he ) { asset_release(); }
    
    virtual bool handle_draw   (HELEMENT he, DRAW_PARAMS& params ) 
    {
      if( params.cmd != DRAW_CONTENT ) return false; // drawing only content layer

      unsigned w = unsigned(params.area.right - params.area.left);
      unsigned h = unsigned(params.area.bottom - params.area.top);

      dib32 buffer(w,h);

      do_draw(buffer.DC(), w,h);

      sciter::image img = sciter::image::create(w,h,false,buffer.bytes());

      sciter::graphics gfx(params.gfx);

      gfx.draw_image(&img, POS(params.area.left), POS(params.area.top));

      return true;
    
    }

    void do_draw( HDC hdc, unsigned width, unsigned height ) {
      RECT rc = { 0,0,int(width),int(height) };
      HBRUSH hbr1 = ::CreateSolidBrush(0x000000);
      HBRUSH hbr2 = ::CreateSolidBrush(0x0000FF);
      // fill it by black color:
      ::FillRect(hdc,&rc,hbr1);
      ::InflateRect(&rc,-40,-40);
      ::FillRect(hdc,&rc,hbr2);
      ::DeleteObject(hbr1);
      ::DeleteObject(hbr2);
    }

};

struct gdi_drawing_factory: public behavior_factory {

  gdi_drawing_factory(): behavior_factory("gdi-drawing") { }

  // the only behavior_factory method:
  virtual event_handler* create(HELEMENT he) {
    return new gdi_drawing();
  }

};

// instantiating and attaching it to the global list
gdi_drawing_factory gdi_drawing_factory_instance;


}
