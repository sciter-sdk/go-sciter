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
 * Sciter's platform independent graphics interface. Used in custom behaviors / event handlers to draw on element's surface in native code. 
 * 
 * Essentially this mimics Graphics object as close as possible. 
 *
 * C++ wrappers
 */


#ifndef __sciter_graphin_hpp__
#define __sciter_graphin_hpp__

#include "sciter-x-graphics.h"
#include "sciter-x-api.h"

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

#include "aux-slice.h"

namespace sciter
{
  typedef SC_COLOR COLOR;
  typedef SC_COLOR_STOP COLOR_STOP;
  typedef SC_DIM   DIM;
  typedef SC_POS   POS;
  typedef SC_ANGLE ANGLE;

  struct writer
  {
    virtual bool write( aux::bytes data ) = 0; // redefine to do actual writing of data.start/data.length
    static SBOOL SCAPI image_write_function(LPVOID prm, const BYTE* data, UINT data_length)
    {
      writer* pw = (writer* )prm;
      return pw->write( aux::bytes(data,data_length) );
    }
  };

  struct bytes_writer: public writer // helper functor
  {
    pod::byte_buffer bb;
    inline virtual bool write( aux::bytes data ) { bb.push(data.start, data.length); return true; }
    aux::bytes bytes() const { return aux::bytes(bb.data(), bb.length()); }
  };

  class graphics;
  class painter;

  class image
  {
    friend class graphics;
  protected:
    HIMG himg;
    
    image(HIMG h): himg(h) { ; }
    
  public:

    image(): himg(0) { ; }
    image(const image& im): himg(im.himg) { if(himg) gapi()->imageAddRef(himg); }
    image& operator = (const image& im) 
    { 
      if(himg) gapi()->imageRelease(himg); 
      himg = im.himg; if(himg) gapi()->imageAddRef(himg); 
      return *this;
    }
    
    bool is_valid() const { return himg != 0; }

    static image create( UINT width, UINT height, bool withAlpha )
    {
      HIMG himg = 0;
      GRAPHIN_RESULT r = gapi()->imageCreate(&himg, width, height, withAlpha ); assert(r == GRAPHIN_OK); (void)(r);
      if( himg ) 
        return image( himg );
      return image(0);
    }
    // create image from BGRA data. Size of pixmap is width*height*4 bytes. 
    static image create( UINT width, UINT height, bool withAlpha, const BYTE* pixmap )
    {
      HIMG himg = 0;
      GRAPHIN_RESULT r = gapi()->imageCreateFromPixmap(&himg, width, height, withAlpha, pixmap ); assert(r == GRAPHIN_OK); (void)(r);
      if( himg ) 
        return image( himg );
      return image(0);
    }
    static image load( aux::bytes data ) // loads image from png or jpeg encoded data
    {
      HIMG himg;
      GRAPHIN_RESULT r = gapi()->imageLoad( data.start, UINT(data.length), &himg); assert(r == GRAPHIN_OK); (void)(r);
      if( himg )
        return image( himg );
      return image(0);
    }

    // fetch image reference from sciter::value envelope
    static image from(const sciter::value& valImage) {
      HIMG himg;
      GRAPHIN_RESULT r = gapi()->vUnWrapImage(&valImage, &himg); assert(r == GRAPHIN_OK); (void)(r);
      if (himg) {
        gapi()->imageAddRef(himg);
        return image(himg);
      }
      return image(0);
    }

    sciter::value to_value() {
      sciter::value v;
      GRAPHIN_RESULT r = gapi()->vWrapImage(himg,&v); assert(r == GRAPHIN_OK); (void)(r);
      return v;
    }

    void paint( painter* p );

    void save( writer& w, SCITER_IMAGE_ENCODING encoding, UINT quality = 0 /*JPEG qquality: 20..100, if 0 - PNG */ ) // save image as png or jpeg enoded data
    {
      GRAPHIN_RESULT r = gapi()->imageSave( himg, writer::image_write_function, &w, encoding, quality );
      assert(r == GRAPHIN_OK); (void)(r);
    }

    ~image() 
    {
      if( himg ) {
        GRAPHIN_RESULT r = gapi()->imageRelease( himg ); assert(r == GRAPHIN_OK); (void)(r);
      }
    }
    
    bool dimensions( UINT& width, UINT& height ) {
      if( himg ) {
        SBOOL usesAlpha;
        GRAPHIN_RESULT r = gapi()->imageGetInfo(himg,&width,&height,&usesAlpha); assert(r == GRAPHIN_OK); (void)(r);
        return true;
      }
      width = 0; height = 0;
      return false;
    }

    void clear(COLOR c = 0)
    {
        GRAPHIN_RESULT r = gapi()->imageClear(himg,c); 
        assert(r == GRAPHIN_OK); (void)(r);
    }

  };

  inline COLOR gcolor(UINT r, UINT g, UINT b, UINT a = 255) { return gapi()->RGBA( r, g, b, a ); }

  // graphic path object 
  class path
  {
    friend class graphics;
  protected:
    HPATH hpath;

    path(HPATH h): hpath(h) { }
  public:
    path(): hpath(0) { }
    path(const path& pa): hpath(pa.hpath) { if(hpath) gapi()->pathAddRef(hpath); }
    path& operator = (const path& pa) 
    { 
      if(hpath) gapi()->pathRelease(hpath); 
      hpath = pa.hpath; gapi()->pathAddRef(hpath); 
      return *this;
    }
    
    bool is_valid() const { return hpath != 0; }

    static path create()
    {
      HPATH hpath = 0;
      GRAPHIN_RESULT r = gapi()->pathCreate(&hpath); assert(r == GRAPHIN_OK); (void)(r);
      return path( hpath );
    }

    // fetch path reference from sciter::value envelope
    static path from(const sciter::value& valPath) {
      HPATH hpath;
      GRAPHIN_RESULT r = gapi()->vUnWrapPath(&valPath, &hpath); assert(r == GRAPHIN_OK); (void)(r);
      if (hpath) {
        return path(hpath);
      }
      return path(0);
    }

    sciter::value to_value() {
      sciter::value v;
      GRAPHIN_RESULT r = gapi()->vWrapPath(hpath,&v); assert(r == GRAPHIN_OK); (void)(r);
      return v;
    }

    ~path() 
    {
      if( hpath ) {
        GRAPHIN_RESULT r = gapi()->pathRelease( hpath ); assert(r == GRAPHIN_OK); (void)(r);
      }
    }

    void move_to ( POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathMoveTo( hpath, x, y, relative ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void line_to ( POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathLineTo( hpath, x, y, relative ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void arc_to( POS x, POS y, ANGLE angle, POS rx, POS ry, bool is_large_arc, bool sweep_flag, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathArcTo( hpath, x, y, angle, rx, ry, is_large_arc, sweep_flag, relative ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void quadratic_curve_to ( POS xc, POS yc, POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathQuadraticCurveTo( hpath, xc, yc, x, y, relative ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void bezier_curve_to ( POS xc1, POS yc1, POS xc2, POS yc2, POS x, POS y, bool relative )
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathBezierCurveTo( hpath, xc1, yc1, xc2, yc2, x, y, relative ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void close_path()
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathClosePath( hpath ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
  };


  // graphic path object 
  class text
  {
    friend class graphics;
  protected:
    HTEXT htext;

    text(HTEXT h) : htext(h) { }
  public:
    text(aux::wchars chars, HELEMENT he, LPCWSTR className = NULL) : htext(0) {
      assert(he);
      GRAPHIN_RESULT r = gapi()->textCreateForElement(&htext, chars.start, UINT(chars.length), he, className);
      assert(r == GRAPHIN_OK); (void)(r);
    }
            
    text(const text& pa) : htext(pa.htext) { if (htext) gapi()->textAddRef(htext); }

    static text create_with_style(aux::wchars chars, HELEMENT he, aux::wchars style) {
      HTEXT htext;
      GRAPHIN_RESULT r = gapi()->textCreateForElementAndStyle(&htext, chars.start, UINT(chars.length), he, style.start, UINT(style.length));
      assert(r == GRAPHIN_OK); (void)(r);
      return text(htext);
    }

    ~text()
    {
      if (htext) {
        GRAPHIN_RESULT r = gapi()->textRelease(htext); assert(r == GRAPHIN_OK); (void)(r);
      }
    }

    text& operator = (const text& pa)
    {
      if (htext) gapi()->textRelease(htext);
      htext = pa.htext; gapi()->textAddRef(htext);
      return *this;
    }

    bool is_valid() const { return htext != 0; }

    // fetch text reference from sciter::value envelope
    static text from(const sciter::value& valPath) {
      HTEXT htext;
      GRAPHIN_RESULT r = gapi()->vUnWrapText(&valPath, &htext); assert(r == GRAPHIN_OK); (void)(r);
      if (htext) {
        return text(htext);
      }
      return text(0);
    }

    sciter::value to_value() {
      sciter::value v;
      GRAPHIN_RESULT r = gapi()->vWrapText(htext, &v); assert(r == GRAPHIN_OK); (void)(r);
      return v;
    }

    /*void bezier_curve_to(POS xc1, POS yc1, POS xc2, POS yc2, POS x, POS y, bool relative)
    {
      assert(hpath);
      GRAPHIN_RESULT r = gapi()->pathBezierCurveTo(hpath, xc1, yc1, xc2, yc2, x, y, relative);
      assert(r == GRAPHIN_OK); (void)(r);
    }*/

    void get_metrics(SC_DIM *minWidth,
      SC_DIM *maxWidth,
      SC_DIM *height,
      SC_DIM *ascent,
      SC_DIM *descent,
      UINT *nLines) {
      assert(htext);
      GRAPHIN_RESULT r = gapi()->textGetMetrics(htext, minWidth, maxWidth, height, ascent, descent, nLines);
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void set_box(SC_DIM width, SC_DIM height) {
      assert(htext);
      GRAPHIN_RESULT r = gapi()->textSetBox(htext, width, height);
      assert(r == GRAPHIN_OK); (void)(r);
    }
  };
  
  class graphics
  {
    HGFX hgfx;  
  public:

    graphics( HGFX gfx ): hgfx(gfx) { if(hgfx) gapi()->gAddRef(hgfx); }
    
    ~graphics() { if(hgfx) gapi()->gRelease(hgfx); }

    // fetch graphics reference from sciter::value envelope
    static graphics from(const sciter::value& valGfx) {
      HGFX hgfx;  
      GRAPHIN_RESULT r = gapi()->vUnWrapGfx(&valGfx, &hgfx); assert(r == GRAPHIN_OK); (void)(r);
      if (hgfx) {
        //-- gapi()->gAddRef(hgfx); - ctor will do that
        return graphics(hgfx);
      }
      return graphics(0);
    }

    sciter::value to_value() {
      sciter::value v;
      GRAPHIN_RESULT r = gapi()->vWrapGfx(hgfx,&v); assert(r == GRAPHIN_OK); (void)(r);
      return v;
    }

    // Draws line from x1,y1 to x2,y2 using current line_color and line_gradient.
    void line ( POS x1, POS y1, POS x2, POS y2 )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLine( hgfx, x1, y1, x2, y2 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws rectangle using current lineColor/lineGradient and fillColor/fillGradient 
    void rectangle ( POS x1, POS y1, POS x2, POS y2 )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRectangle( hgfx, x1, y1, x2, y2 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws rounded rectangle using current lineColor/lineGradient and fillColor/fillGradient with rounded corners.
    void rectangle ( POS x1, POS y1, POS x2, POS y2, DIM rAll )
    {
      DIM rad[8] = { rAll, rAll, rAll, rAll, rAll, rAll, rAll, rAll};
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRoundedRectangle( hgfx, x1, y1, x2, y2, rad ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void rectangle ( POS x1, POS y1, POS x2, POS y2, DIM rTopLeft, DIM rTopRight, DIM rBottomRight, DIM rBottomLeft )
    {
      DIM rad[8] = { rTopLeft, rTopLeft, rTopRight, rTopRight, rBottomRight, rBottomRight, rBottomLeft, rBottomLeft };
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRoundedRectangle( hgfx, x1, y1, x2, y2, rad ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws circle or ellipse using current lineColor/lineGradient and fillColor/fillGradient.
    void ellipse ( POS x, POS y, POS rx, POS ry )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gEllipse( hgfx, x, y, rx, ry ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    void circle ( POS x, POS y, POS radii )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gEllipse( hgfx, x, y, radii, radii ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws closed arc using current lineColor/lineGradient and fillColor/fillGradient.
    void arc ( POS x, POS y, POS rx, POS ry, ANGLE start, ANGLE sweep )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gArc( hgfx, x, y, rx, ry, start, sweep ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws star.
    void star ( POS x, POS y, POS r1, POS r2, ANGLE start, UINT rays )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStar( hgfx, x, y, r1, r2, start, rays ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // Draws closed polygon using current lineColor/lineGradient and fillColor/fillGradient.
    void polygon ( POS* xy, UINT num_points )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPolygon( hgfx, xy, num_points ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // you bet
    void polyline ( POS* xy, unsigned int num_points )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPolyline( hgfx, xy, num_points ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // SECTION: Path operations

    void draw_path(const path& p, DRAW_PATH_MODE dpm)
    {
      assert(hgfx && p.hpath);
      GRAPHIN_RESULT r = gapi()->gDrawPath( hgfx, p.hpath, dpm ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // end of path opearations

    // SECTION: affine tranformations:

    void rotate ( ANGLE radians )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRotate( hgfx, radians, 0, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void rotate ( ANGLE radians, POS center_x, POS center_y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gRotate( hgfx, radians, &center_x, &center_y ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void translate ( POS cx, POS cy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTranslate( hgfx, cx, cy ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void scale ( SC_REAL x, SC_REAL y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScale( hgfx, x, y ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void skew ( SC_REAL dx, SC_REAL dy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gSkew( hgfx, dx, dy ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // all above in one shot
    void transform ( POS m11, POS m12, POS m21, POS m22, POS dx, POS dy )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTransform( hgfx, m11, m12, m21, m22, dx, dy ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // end of affine tranformations.

    // SECTION: state save/restore

    void state_save ( )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStateSave( hgfx ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void state_restore ( )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gStateRestore( hgfx ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // end of state save/restore

    // SECTION: drawing attributes

    // set line width for subsequent drawings.
    void line_width ( DIM width )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineWidth( hgfx, width ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void no_line ( ) 
    { 
      line_width(0.0); 
    }

    void line_color ( COLOR c ) 
    { 
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineColor( hgfx, c); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void line_cap(SCITER_LINE_CAP_TYPE ct)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineCap( hgfx, ct); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void line_join(SCITER_LINE_JOIN_TYPE jt)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineJoin( hgfx, jt); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // COLOR for solid fills
    void fill_color ( COLOR c )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillColor( hgfx, c); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void no_fill ( ) 
    { 
      fill_color ( COLOR(0) );
    }

    // setup parameters of linear gradient of lines.
    void line_linear_gradient( POS x1, POS y1, POS x2, POS y2, const COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineGradientLinear( hgfx, x1, y1, x2, y2, stops, nstops ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    void line_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR c1, COLOR c2 )
    {
      const COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      line_linear_gradient( x1, y1, x2, y2, stops, 2 );
    }

    // setup parameters of linear gradient of fills.
    void fill_linear_gradient( POS x1, POS y1, POS x2, POS y2, const COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillGradientLinear( hgfx, x1, y1, x2, y2, stops, nstops); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void fill_linear_gradient( POS x1, POS y1, POS x2, POS y2, COLOR c1, COLOR c2 )
    {
      const COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      fill_linear_gradient( x1, y1, x2, y2, stops, 2 );
    }

    // setup parameters of line gradient radial fills.
    void line_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, const COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gLineGradientRadial( hgfx, x, y, radiix, radiiy, stops, nstops); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void line_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR c1, COLOR c2 )
    {
      const COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      line_radial_gradient( x, y, radiix,radiiy, stops, 2 );
    }

    // setup parameters of gradient radial fills.
    void fill_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, const COLOR_STOP* stops, UINT nstops )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillGradientRadial( hgfx, x, y, radiix, radiiy, stops, nstops); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void fill_radial_gradient( POS x, POS y, DIM radiix, DIM radiiy, COLOR c1, COLOR c2 )
    {
      const COLOR_STOP stops[2] = { {c1, 0.0}, {c2, 1.0} };
      fill_radial_gradient( x, y, radiix, radiiy, stops, 2 );
    }
    
    void fill_mode( bool even_odd /* false - fill_non_zero */ )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFillMode( hgfx, even_odd ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // SECTION: text
/*
    // define font attributes for all subsequent text operations.
    void font(  const char* name, DIM size, bool bold = false, bool italic = false, ANGLE angle = 0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gSetFont( hgfx, name, size, bold, italic, angle ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // draw text at x,y with text alignment
    void text(POS x, POS y, aux::wchars t)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gText( hgfx, x, y, t.start, t.length ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // calculates width of the text string
    DIM text_width( aux::wchars t )
    {
      DIM width;
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTextWidth( hgfx, t.start, t.length, &width ); 
      assert(r == GRAPHIN_OK); (void)(r);
      return width;
    }

    // returns height and ascent of the font.
    void font_metrics( DIM& height, DIM& ascent )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFontMetrics( hgfx, &height, &ascent ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // set text alignments
    void text_alignment( SCITER_TEXT_ALIGNMENT x, SCITER_TEXT_ALIGNMENT y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gTextAlignment( hgfx, x, y ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
*/

    // SECTION: image rendering

    // draws img onto the graphics surface with current transformation applied (scale, rotation). expensive
    void draw_image ( const image* pimg, POS x, POS y, DIM w, DIM h, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, &w, &h, &ix, &iy, &iw, &ih, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void draw_image ( const image& pimg, POS x, POS y, DIM w, DIM h, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg.himg, x, y, &w, &h, &ix, &iy, &iw, &ih, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // draws whole img onto the graphics surface with current transformation applied (scale, rotation). expensive
    void draw_image ( const image* pimg, POS x, POS y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, 0, 0, 0, 0, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    void draw_image ( const image& pimg, POS x, POS y )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg.himg, x, y, 0, 0, 0, 0, 0, 0, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    // blends image bits with bits of the surface. no affine transformations. less expensive
    void blend_image ( const image* pimg, POS x, POS y, float opacity, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, &ix, &iy, &iw, &ih, &opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    void blend_image ( const image& pimg, POS x, POS y, float opacity, UINT ix, UINT iy, UINT iw, UINT ih )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg.himg, x, y, 0, 0, &ix, &iy, &iw, &ih, &opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    // blends image bits with bits of the surface. no affine transformations. less expensive
    void blend_image ( const image* pimg, POS x, POS y, float opacity )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg->himg, x, y, 0, 0, 0, 0, 0, 0, &opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }
    void blend_image ( const image& pimg, POS x, POS y, float opacity )
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawImage( hgfx, pimg.himg, x, y, 0, 0, 0, 0, 0, 0, &opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void draw_text(const text& ptext, POS x, POS y, UINT ref = 7 /*x/y is top/left*/)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gDrawText(hgfx, ptext.htext, x, y, ref);
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // SECTION: coordinate space

    void world_to_screen ( POS& inout_x, POS& inout_y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gWorldToScreen( hgfx, &inout_x, &inout_y ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void world_to_screen ( DIM& inout_length)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gWorldToScreen( hgfx, &inout_length, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void screen_to_world ( POS& inout_x, POS& inout_y)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScreenToWorld( hgfx, &inout_x, &inout_y ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void screen_to_world ( DIM& inout_length)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gScreenToWorld( hgfx, &inout_length, 0 ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    // SECTION: clipping

    void push_clip_box ( POS x1, POS y1, POS x2, POS y2, float opacity = 1.0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPushClipBox( hgfx, x1, y1, x2, y2, opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void push_clip_path ( const path& p, float opacity = 1.0)
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPushClipPath( hgfx, p.hpath , opacity ); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void pop_clip ()
    {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gPopClip( hgfx); 
      assert(r == GRAPHIN_OK); (void)(r);
    }

    void flush() {
      assert(hgfx);
      GRAPHIN_RESULT r = gapi()->gFlush(hgfx);
      assert(r == GRAPHIN_OK); (void)(r);
    }
  };

  class painter
  {
    friend class image;
  public:
    virtual void paint( graphics& gfx, UINT width, UINT height ) = 0; // redefine to do actual painting on image
  protected:
    static VOID SCAPI image_paint_function(LPVOID prm, HGFX hgfx, UINT width, UINT height)
    {
      painter* pp = (painter* )prm;
      graphics gfx(hgfx);
      pp->paint( gfx, width, height );
    }
  };

  inline void image::paint( painter* p ) {
    assert(himg);
    GRAPHIN_RESULT r = gapi()->imagePaint(himg, &painter::image_paint_function,p);
    assert(r == GRAPHIN_OK); (void)(r);
  } 

}

#endif //defined(__cplusplus) && !defined( PLAIN_API_ONLY )


#endif


