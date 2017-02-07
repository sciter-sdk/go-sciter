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
enum SCITER_X_MSG_CODE {
  SXM_CREATE  = 0,
  SXM_DESTROY = 1,
  SXM_SIZE    = 2,
  SXM_PAINT   = 3,
};

/** #SCITER_X_MSG common header of message structures passed to SciterProcX */
struct SCITER_X_MSG
{
  UINT msg;  /**< [in] one of the codes of #SCITER_X_MSG_CODE.*/
#ifdef __cplusplus
  SCITER_X_MSG(UINT m) : msg(m) {}
#endif
};

struct SCITER_X_MSG_CREATE
{
  SCITER_X_MSG header;
          UINT backend;
          BOOL transparent;
#ifdef __cplusplus
   SCITER_X_MSG_CREATE(UINT backendType = GFX_LAYER_SKIA_OPENGL, BOOL isTransparent = TRUE )
     : header(SXM_CREATE), backend(backendType), transparent(isTransparent) {}
#endif
};

struct SCITER_X_MSG_DESTROY {
  SCITER_X_MSG header;
#ifdef __cplusplus
  SCITER_X_MSG_DESTROY() : header(SXM_DESTROY) {}
#endif
};

struct SCITER_X_MSG_SIZE {
  SCITER_X_MSG header;
          UINT width;
          UINT height;
#ifdef __cplusplus
  SCITER_X_MSG_SIZE(UINT w, UINT h) : header(SXM_SIZE), width(w), height(h) {}
#endif
};


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
enum SCITER_PAINT_TARGET_TYPE {
  SPT_DEFAULT   = 0,  /**< default rendering target - window surface */
  SPT_RECEIVER  = 1,  /**< target::receiver fields are valid */
  SPT_DC        = 2,  /**< target::dc is valid */
  // ...
};

struct SCITER_X_MSG_PAINT {
  SCITER_X_MSG header;
      HELEMENT element;    /**< [in] layer #HELEMENT, can be NULL if whole tree (document) needs to be rendered.*/
      BOOL     isFore;     /**< [in] if element is not null tells if that element is fore-layer.*/
      UINT     targetType; /**< [in] one of #SCITER_PAINT_TARGET_TYPE values */
      union {
        HDC hdc;
        struct {
          VOID*                    param;
          ELEMENT_BITMAP_RECEIVER* callback;
        } receiver;
      } target;

#ifdef __cplusplus
  SCITER_X_MSG_PAINT(HELEMENT layerElement = NULL, BOOL foreLayer = TRUE) : header(SXM_PAINT), element(layerElement), isFore(foreLayer), targetType(SPT_DEFAULT) {}
#endif
};

#endif
