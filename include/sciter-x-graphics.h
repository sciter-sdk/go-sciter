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
 */


#ifndef __sciter_graphin_h__
#define __sciter_graphin_h__

#include "sciter-x-types.h"

#ifdef __cplusplus

namespace gool
{
  class graphics;
  class image;
  class path;
  class text_layout;
}

typedef gool::graphics*     HGFX;
typedef gool::image*        HIMG;
typedef gool::path*         HPATH;
typedef gool::text_layout*  HTEXT;

#else

typedef void*               HGFX;
typedef void*               HIMG;
typedef void*               HPATH;
typedef void*               HTEXT;

#endif

typedef float SC_REAL;

typedef SC_REAL  SC_POS;        // position
typedef SC_REAL  SC_DIM;        // dimension
typedef SC_REAL  SC_ANGLE;      // angle (radians)
typedef unsigned int SC_COLOR;  // color

typedef struct SC_COLOR_STOP
{
  SC_COLOR color;
  float offset; // 0.0 ... 1.0
} SC_COLOR_STOP;

typedef enum GRAPHIN_RESULT
{
  GRAPHIN_PANIC = -1, // e.g. not enough memory
  GRAPHIN_OK = 0,
  GRAPHIN_BAD_PARAM = 1,  // bad parameter
  GRAPHIN_FAILURE = 2,    // operation failed, e.g. restore() without save()
  GRAPHIN_NOTSUPPORTED = 3 // the platform does not support requested feature
} GRAPHIN_RESULT;

typedef enum DRAW_PATH_MODE
{
  DRAW_FILL_ONLY = 1,
  DRAW_STROKE_ONLY = 2,
  DRAW_FILL_AND_STROKE = 3,
} DRAW_PATH_MODE;

typedef enum SCITER_LINE_JOIN_TYPE
{
  SCITER_JOIN_MITER = 0,
  SCITER_JOIN_ROUND = 1,
  SCITER_JOIN_BEVEL = 2,
  SCITER_JOIN_MITER_OR_BEVEL = 3,
} SCITER_LINE_JOIN_TYPE;

typedef enum SCITER_LINE_CAP_TYPE
{
  SCITER_LINE_CAP_BUTT = 0,
  SCITER_LINE_CAP_SQUARE = 1,
  SCITER_LINE_CAP_ROUND = 2,
} SCITER_LINE_CAP_TYPE;

/*typedef enum SCITER_TEXT_ALIGNMENT
{
  TEXT_ALIGN_DEFAULT,
  TEXT_ALIGN_START,
  TEXT_ALIGN_END,
  TEXT_ALIGN_CENTER,
} SCITER_TEXT_ALIGNMENT;

typedef enum SCITER_TEXT_DIRECTION
{
  TEXT_DIRECTION_DEFAULT,
  TEXT_DIRECTION_LTR,
  TEXT_DIRECTION_RTL,
  TEXT_DIRECTION_TTB,
} SCITER_TEXT_DIRECTION; */

typedef enum SCITER_IMAGE_ENCODING
{
  SCITER_IMAGE_ENCODING_RAW, // [a,b,g,r,a,b,g,r,...] vector
  SCITER_IMAGE_ENCODING_PNG,
  SCITER_IMAGE_ENCODING_JPG,
  SCITER_IMAGE_ENCODING_WEBP,
} SCITER_IMAGE_ENCODING;

/* typedef struct SCITER_TEXT_FORMAT
{
    LPWSTR                fontFamily;
    UINT                  fontWeight; // 100...900, 400 - normal, 700 - bold
    SBOOL                  fontItalic;
    float                 fontSize;   // dips
    float                 lineHeight; // dips
    SCITER_TEXT_DIRECTION textDirection;
    SCITER_TEXT_ALIGNMENT textAlignment; // horizontal alignment
    SCITER_TEXT_ALIGNMENT lineAlignment; // a.k.a. vertical alignment for roman writing systems
    LPWSTR                localeName;
} SCITER_TEXT_FORMAT; */

// imageSave callback:
typedef SBOOL SCAPI image_write_function(LPVOID prm, const BYTE* data, UINT data_length);
// imagePaint callback:
typedef VOID SCAPI image_paint_function(LPVOID prm, HGFX hgfx, UINT width, UINT height);

struct SciterGraphicsAPI
{
// image primitives
  GRAPHIN_RESULT
        SCFN(imageCreate)( HIMG* poutImg, UINT width, UINT height, SBOOL withAlpha );

  // construct image from B[n+0],G[n+1],R[n+2],A[n+3] data.
  // Size of pixmap data is pixmapWidth*pixmapHeight*4
  GRAPHIN_RESULT
        SCFN(imageCreateFromPixmap)( HIMG* poutImg, UINT pixmapWidth, UINT pixmapHeight, SBOOL withAlpha, const BYTE* pixmap );

  GRAPHIN_RESULT
        SCFN(imageAddRef)( HIMG himg );

  GRAPHIN_RESULT
        SCFN(imageRelease)( HIMG himg );

  GRAPHIN_RESULT
        SCFN(imageGetInfo)( HIMG himg,
             UINT* width,
             UINT* height,
             SBOOL* usesAlpha );

  //GRAPHIN_RESULT
  //      SCFN(imageGetPixels)( HIMG himg,
  //           image_write_function* dataReceiver );

  GRAPHIN_RESULT
        SCFN(imageClear)( HIMG himg, SC_COLOR byColor );

  GRAPHIN_RESULT
        SCFN(imageLoad)( const BYTE* bytes, UINT num_bytes, HIMG* pout_img ); // load png/jpeg/etc. image from stream of bytes

  GRAPHIN_RESULT
        SCFN(imageSave) // save png/jpeg/etc. image to stream of bytes
          ( HIMG himg,
          image_write_function* pfn, void* prm, /* function and its param passed "as is" */
          UINT encoding, // SCITER_IMAGE_ENCODING
          UINT quality /* if webp or jpeg:, 10 - 100 */ );
  
  // SECTION: graphics primitives and drawing operations

  // create SC_COLOR value
  SC_COLOR
        SCFN(RGBA)(UINT red, UINT green, UINT blue, UINT alpha /*= 255*/);

  GRAPHIN_RESULT
        SCFN(gCreate)(HIMG img, HGFX* pout_gfx );

  GRAPHIN_RESULT
        SCFN(gAddRef) (HGFX gfx);

  GRAPHIN_RESULT
        SCFN(gRelease) (HGFX gfx);

// Draws line from x1,y1 to x2,y2 using current lineColor and lineGradient.
  GRAPHIN_RESULT
        SCFN(gLine) ( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2 );

// Draws rectangle using current lineColor/lineGradient and fillColor/fillGradient with (optional) rounded corners.
  GRAPHIN_RESULT
        SCFN(gRectangle) ( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2 );

// Draws rounded rectangle using current lineColor/lineGradient and fillColor/fillGradient with (optional) rounded corners.
  GRAPHIN_RESULT
        SCFN(gRoundedRectangle) ( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2, const SC_DIM* radii8 /*SC_DIM[8] - four rx/ry pairs */);

// Draws circle or ellipse using current lineColor/lineGradient and fillColor/fillGradient.
  GRAPHIN_RESULT
        SCFN(gEllipse) ( HGFX hgfx, SC_POS x, SC_POS y, SC_DIM rx, SC_DIM ry );

// Draws closed arc using current lineColor/lineGradient and fillColor/fillGradient.
  GRAPHIN_RESULT
        SCFN(gArc) ( HGFX hgfx, SC_POS x, SC_POS y, SC_POS rx, SC_POS ry, SC_ANGLE start, SC_ANGLE sweep );

// Draws star.
  GRAPHIN_RESULT
        SCFN(gStar) ( HGFX hgfx, SC_POS x, SC_POS y, SC_DIM r1, SC_DIM r2, SC_ANGLE start, UINT rays );

// Closed polygon.
  GRAPHIN_RESULT
        SCFN(gPolygon) ( HGFX hgfx, const SC_POS* xy, UINT num_points );

  // Polyline.
  GRAPHIN_RESULT
        SCFN(gPolyline) ( HGFX hgfx, const SC_POS* xy, UINT num_points );

// SECTION: Path operations

  GRAPHIN_RESULT
        SCFN(pathCreate) ( HPATH* path );

  GRAPHIN_RESULT
        SCFN(pathAddRef) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(pathRelease) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(pathMoveTo) ( HPATH path, SC_POS x, SC_POS y, SBOOL relative );

  GRAPHIN_RESULT
        SCFN(pathLineTo) ( HPATH path, SC_POS x, SC_POS y, SBOOL relative );

  GRAPHIN_RESULT
        SCFN(pathArcTo) ( HPATH path, SC_POS x, SC_POS y, SC_ANGLE angle, SC_DIM rx, SC_DIM ry, SBOOL is_large_arc, SBOOL clockwise, SBOOL relative );

  GRAPHIN_RESULT
        SCFN(pathQuadraticCurveTo) ( HPATH path, SC_POS xc, SC_POS yc, SC_POS x, SC_POS y, SBOOL relative );

  GRAPHIN_RESULT
        SCFN(pathBezierCurveTo) ( HPATH path, SC_POS xc1, SC_POS yc1, SC_POS xc2, SC_POS yc2, SC_POS x, SC_POS y, SBOOL relative );

  GRAPHIN_RESULT
        SCFN(pathClosePath) ( HPATH path );

  GRAPHIN_RESULT
        SCFN(gDrawPath) ( HGFX hgfx, HPATH path, DRAW_PATH_MODE dpm );

// end of path opearations

// SECTION: affine tranformations:

  GRAPHIN_RESULT
        SCFN(gRotate) ( HGFX hgfx, SC_ANGLE radians, const SC_POS* cx /*= 0*/, const SC_POS* cy /*= 0*/ );

  GRAPHIN_RESULT
        SCFN(gTranslate) ( HGFX hgfx, SC_POS cx, SC_POS cy );

  GRAPHIN_RESULT
        SCFN(gScale) ( HGFX hgfx, SC_DIM x, SC_DIM y );

  GRAPHIN_RESULT
        SCFN(gSkew) ( HGFX hgfx, SC_DIM dx, SC_DIM dy );

  // all above in one shot
  GRAPHIN_RESULT
        SCFN(gTransform) ( HGFX hgfx, SC_POS m11, SC_POS m12, SC_POS m21, SC_POS m22, SC_POS dx, SC_POS dy );

// end of affine tranformations.

// SECTION: state save/restore

  GRAPHIN_RESULT
        SCFN(gStateSave) ( HGFX hgfx );

  GRAPHIN_RESULT
        SCFN(gStateRestore) ( HGFX hgfx );

// end of state save/restore

// SECTION: drawing attributes

  // set line width for subsequent drawings.
  GRAPHIN_RESULT
        SCFN(gLineWidth) ( HGFX hgfx, SC_DIM width );

  GRAPHIN_RESULT
        SCFN(gLineJoin) ( HGFX hgfx, SCITER_LINE_JOIN_TYPE type );

  GRAPHIN_RESULT
        SCFN(gLineCap) ( HGFX hgfx, SCITER_LINE_CAP_TYPE type);

  //GRAPHIN_RESULT SCFN
  //      (*gNoLine ( HGFX hgfx ) { gLineWidth(hgfx,0.0); }

  // SC_COLOR for solid lines/strokes
  GRAPHIN_RESULT
        SCFN(gLineColor) ( HGFX hgfx, SC_COLOR c);

  // SC_COLOR for solid fills
  GRAPHIN_RESULT
        SCFN(gFillColor) ( HGFX hgfx, SC_COLOR color );

//inline void
//      graphics_no_fill ( HGFX hgfx ) { graphics_fill_color(hgfx, graphics_rgbt(0,0,0,0xFF)); }

  // setup parameters of linear gradient of lines.
  GRAPHIN_RESULT
        SCFN(gLineGradientLinear)( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2, const SC_COLOR_STOP* stops, UINT nstops );

  // setup parameters of linear gradient of fills.
  GRAPHIN_RESULT
        SCFN(gFillGradientLinear)( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2, const SC_COLOR_STOP* stops, UINT nstops );

  // setup parameters of line gradient radial fills.
  GRAPHIN_RESULT
        SCFN(gLineGradientRadial)( HGFX hgfx, SC_POS x, SC_POS y, SC_DIM rx, SC_DIM ry, const SC_COLOR_STOP* stops, UINT nstops );

  // setup parameters of gradient radial fills.
  GRAPHIN_RESULT
        SCFN(gFillGradientRadial)( HGFX hgfx, SC_POS x, SC_POS y, SC_DIM rx, SC_DIM ry, const SC_COLOR_STOP* stops, UINT nstops );

  GRAPHIN_RESULT
        SCFN(gFillMode) ( HGFX hgfx, SBOOL even_odd /* false - fill_non_zero */ );

// SECTION: text

  // create text layout for host element
  GRAPHIN_RESULT
        SCFN(textCreateForElement)(HTEXT* ptext, LPCWSTR text, UINT textLength, HELEMENT he, LPCWSTR classNameOrNull );

  // create text layout using explicit style declaration
  GRAPHIN_RESULT
        SCFN(textCreateForElementAndStyle)(HTEXT* ptext, LPCWSTR text, UINT textLength, HELEMENT he, LPCWSTR style, UINT styleLength); 

  GRAPHIN_RESULT
        SCFN(textAddRef) (HTEXT path);

  GRAPHIN_RESULT
        SCFN(textRelease) (HTEXT path);

  GRAPHIN_RESULT
        SCFN(textGetMetrics)(HTEXT text, SC_DIM* minWidth, SC_DIM* maxWidth, SC_DIM* height, SC_DIM* ascent, SC_DIM* descent, UINT* nLines);

  GRAPHIN_RESULT
        SCFN(textSetBox)(HTEXT text, SC_DIM width, SC_DIM height);

  // draw text with position (1..9 on MUMPAD) at px,py
  // Ex: gDrawText( 100,100,5) will draw text box with its center at 100,100 px
  GRAPHIN_RESULT
        SCFN(gDrawText) ( HGFX hgfx, HTEXT text, SC_POS px, SC_POS py, UINT position );

  // SECTION: image rendering

  // draws img onto the graphics surface with current transformation applied (scale, rotation).
  GRAPHIN_RESULT
        SCFN(gDrawImage) ( HGFX hgfx, HIMG himg, SC_POS x, SC_POS y,
                           const SC_DIM* w /*= 0*/, const SC_DIM* h /*= 0*/, 
                           const UINT* ix /*= 0*/, const UINT* iy /*= 0*/, const UINT* iw /*= 0*/, const UINT* ih, /*= 0*/
                           const float* opacity /*= 0, if provided is in 0.0 .. 1.0*/ );

  // SECTION: coordinate space

  GRAPHIN_RESULT
        SCFN(gWorldToScreen) ( HGFX hgfx, SC_POS* inout_x, SC_POS* inout_y);

  GRAPHIN_RESULT
        SCFN(gScreenToWorld) ( HGFX hgfx, SC_POS* inout_x, SC_POS* inout_y);


// SECTION: clipping

  GRAPHIN_RESULT
        SCFN(gPushClipBox) ( HGFX hgfx, SC_POS x1, SC_POS y1, SC_POS x2, SC_POS y2, float opacity /*=1.f*/);

  GRAPHIN_RESULT
        SCFN(gPushClipPath) ( HGFX hgfx, HPATH hpath, float opacity /*=1.f*/);

  // pop clip layer previously set by gPushClipBox or gPushClipPath
  GRAPHIN_RESULT
        SCFN(gPopClip) ( HGFX hgfx);
    
  // image painter

    GRAPHIN_RESULT
        SCFN(imagePaint)( HIMG himg, image_paint_function* pPainter, void* prm ); // paint on image using graphics

  // VALUE interface

  GRAPHIN_RESULT
        SCFN(vWrapGfx) ( HGFX hgfx, VALUE* toValue);

  GRAPHIN_RESULT
        SCFN(vWrapImage) ( HIMG himg, VALUE* toValue);
  
  GRAPHIN_RESULT
        SCFN(vWrapPath) ( HPATH hpath, VALUE* toValue);

  GRAPHIN_RESULT
        SCFN(vWrapText) ( HTEXT htext, VALUE* toValue);

  GRAPHIN_RESULT
        SCFN(vUnWrapGfx) ( const VALUE* fromValue, HGFX *phgfx);

  GRAPHIN_RESULT
        SCFN(vUnWrapImage) ( const VALUE* fromValue, HIMG *phimg );
  
  GRAPHIN_RESULT
        SCFN(vUnWrapPath) ( const VALUE* fromValue, HPATH *phpath);

  GRAPHIN_RESULT
        SCFN(vUnWrapText) ( const VALUE* fromValue, HTEXT *phtext);

  GRAPHIN_RESULT
        SCFN(gFlush) (HGFX hgfx);


};

typedef struct SciterGraphicsAPI* LPSciterGraphicsAPI;

#endif
