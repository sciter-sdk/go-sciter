/*
 * The Sciter Engine of Terra Informatica Software, Inc.
 * http://sciter.com
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * (C) Terra Informatica Software, Inc.
 */

 /*
 *  Message definitions to be passed to SciterProcX function
 */


#ifndef __sciter_x_msg_h__
#define __sciter_x_msg_h__

#include "sciter-x-types.h"
#include "sciter-x-def.h"

/** #SCITER_X_MSG_CODE message/function identifier */
typedef enum SCITER_X_MSG_CODE {
  SXM_CREATE  = 0,
  SXM_DESTROY = 1,
  SXM_SIZE    = 2,
  SXM_PAINT   = 3,
  SXM_RESOLUTION = 4,
  SXM_HEARTBIT = 5,
  SXM_MOUSE = 6,
  SXM_KEY = 7,
  SXM_FOCUS = 8,
} SCITER_X_MSG_CODE;

/** #SCITER_X_MSG common header of message structures passed to SciterProcX */
typedef struct SCITER_X_MSG
{ 
  UINT msg;     /**< [in]  one of the codes of #SCITER_X_MSG_CODE.*/
#ifdef __cplusplus
  SCITER_X_MSG(UINT m) : msg(m) {}
#endif
} SCITER_X_MSG;

typedef struct SCITER_X_MSG_CREATE
{
  SCITER_X_MSG header;
          UINT backend;
          SBOOL transparent;
#ifdef __cplusplus
   SCITER_X_MSG_CREATE(UINT backendType = GFX_LAYER_SKIA_OPENGL, SBOOL isTransparent = TRUE ) 
     : header(SXM_CREATE), backend(backendType), transparent(isTransparent) {}
#endif
} SCITER_X_MSG_CREATE;

typedef struct SCITER_X_MSG_DESTROY {
  SCITER_X_MSG header;
#ifdef __cplusplus
  SCITER_X_MSG_DESTROY() : header(SXM_DESTROY) {}
#endif
} SCITER_X_MSG_DESTROY;

typedef struct SCITER_X_MSG_SIZE {
  SCITER_X_MSG header;
          UINT width;
          UINT height;
#ifdef __cplusplus
  SCITER_X_MSG_SIZE(UINT w, UINT h) : header(SXM_SIZE), width(w), height(h) {}
#endif
} SCITER_X_MSG_SIZE;

typedef struct SCITER_X_MSG_RESOLUTION {
  SCITER_X_MSG header;
  UINT pixelsPerInch;
#ifdef __cplusplus
  SCITER_X_MSG_RESOLUTION(UINT ppi) : header(SXM_RESOLUTION), pixelsPerInch(ppi) {}
#endif
} SCITER_X_MSG_RESOLUTION;

typedef struct SCITER_X_MSG_MOUSE {
  SCITER_X_MSG    header;
  MOUSE_EVENTS    event;
  MOUSE_BUTTONS   button;
  KEYBOARD_STATES modifiers;
  POINT           pos;
#ifdef __cplusplus
  SCITER_X_MSG_MOUSE(MOUSE_EVENTS e, MOUSE_BUTTONS b, KEYBOARD_STATES mods, POINT p) : header(SXM_MOUSE), event(e), button(b), modifiers(mods), pos(p) {}
#endif
} SCITER_X_MSG_MOUSE;

typedef struct SCITER_X_MSG_KEY {
  SCITER_X_MSG    header;
  KEY_EVENTS      event;
  UINT            code;
  KEYBOARD_STATES modifiers;
#ifdef __cplusplus
  SCITER_X_MSG_KEY(KEY_EVENTS e, UINT c, KEYBOARD_STATES mods) : header(SXM_KEY), event(e), code(c), modifiers(mods) {}
#endif
} SCITER_X_MSG_KEY;

typedef struct SCITER_X_MSG_FOCUS {
  SCITER_X_MSG    header;
  SBOOL            got; // true - got, false - lost
#ifdef __cplusplus
  SCITER_X_MSG_FOCUS(SBOOL g) : header(SXM_FOCUS), got(g) {}
#endif
} SCITER_X_MSG_FOCUS;


typedef struct SCITER_X_MSG_HEARTBIT {
  SCITER_X_MSG header;
  UINT time;
#ifdef __cplusplus
  SCITER_X_MSG_HEARTBIT(UINT t) : header(SXM_HEARTBIT), time(t) {}
#endif
} SCITER_X_MSG_HEARTBIT;


/** #ELEMENT_BITMAP_RECEIVER - callback function that receives pointer to pixmap and location
* \param[in] bgra \b LPCBYTE, pointer to BGRA bitmap, number of bytes = width * height * 4
* \param[in] x \b INT, position of the bitmap on elements window.
* \param[in] y \b INT, position of the bitmap on elements window.
* \param[in] width \b UINT, width of bitmap (and element's shape).
* \param[in] height \b UINT, height of bitmap (and element's shape).
* \param[in] param \b LPVOID, param that passed as SCITER_X_MSG_PAINT::receiver::param .
**/
typedef VOID SC_CALLBACK ELEMENT_BITMAP_RECEIVER(LPCBYTE rgba, INT x, INT y, UINT width, UINT height, LPVOID param);

/** #SCITER_X_MSG_PAINT target identifier. */
typedef enum SCITER_PAINT_TARGET_TYPE {
  SPT_DEFAULT   = 0,  /**< default rendering target - window surface */    
  SPT_RECEIVER  = 1,  /**< target::receiver fields are valid */    
  SPT_SURFACE   = 2,  /**< target::pSurface is valid */
  //SPT_OPENGL    = 3,  /**< target is not used - caller shall set current context on its side  */
  //SPT_OPENGLES  = 4,  /**< target is not used - caller shall set current context on its side  */
  //SPT_
} SCITER_PAINT_TARGET_TYPE;

typedef struct SCITER_X_MSG_PAINT {
  SCITER_X_MSG header;
      HELEMENT element;    /**< [in] layer #HELEMENT, can be NULL if whole tree (document) needs to be rendered.*/
      SBOOL     isFore;     /**< [in] if element is not null tells if that element is fore-layer.*/    
      UINT     targetType; /**< [in] one of #SCITER_PAINT_TARGET_TYPE values */
      union {
        LPVOID pSurface;   /**< [in] must be IDXGISurface* */
        struct {
          VOID*                    param;
          ELEMENT_BITMAP_RECEIVER* callback;
        } receiver;
      } target;

#ifdef __cplusplus
  SCITER_X_MSG_PAINT(HELEMENT layerElement = NULL, SBOOL foreLayer = TRUE) : header(SXM_PAINT), element(layerElement), isFore(foreLayer), targetType(SPT_DEFAULT) {}
#endif
} SCITER_X_MSG_PAINT;

#endif
